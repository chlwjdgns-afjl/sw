//#include "stdafx.h"
#include "stdio.h"
#define _USE_MATH_DEFINES
#include "math.h"

#define RAD2DEG(x)   x*180/M_PI  //라이안에더 디글로
#define DEG2RAD(x)   x/180*M_PI  //디글에서 라디안으로

typedef struct
{
	double x;
	double y;
	double yaw;
} Pose2D;

typedef struct
{
	double x;
	double y;
} Point2D;

typedef struct
{
	double x;
	double y;
	double z;
} Point3D;

typedef struct {
	double a;
	double b;
}Line_equation;

Pose2D    base_link_origin;                  //base_link의 원점 pose2D
Pose2D    utm_link_origin;                   //utm      의 원점 pose2D
Pose2D    odom_link_origin;                  //domo     의 원점 pose2D

Point2D   waypoint_utm[15];
Point2D   waypoint_utm_relative[15];          //상대좌표
Point2D   waypoint_map_relative[15];
Point2D   waypoint_utm_datum;                 //utm 기준 좌표

Point2D   base_link_Point2D[15], base_link_map_Point2D[15];
Line_equation waypoint_line_equation_utm[15];  //utm기준 waypoint의 line equation
Line_equation waypoint_line_equation_map[15];  //map기준 waypoint의 line equation
double    waypoint_angle_utm[15] = { 0.0, };
double    waypoint_angle_map[15] = { 0.0, };
double    waypoint_distance[15]  = { 0.0, };

double    angle_degree;
double    angle_radian;

double    Rotation_matrix_point[2][2];
double    Rotation_matrix_aixs[2][2];

int initialize_waypoint(void) {
	FILE* fp;
	fp = fopen("waypoints_data.txt", "r");

	waypoint_utm[0]  = { 403165.834539,  4129142.66722 };
	waypoint_utm[1]  = { 403163.074411,  4129144.6631  };
	waypoint_utm[2]  = { 403168.006126,  4129155.70202 };
	waypoint_utm[3]  = { 403171.55259,   4129159.62227 };
	waypoint_utm[4]  = { 403175.499272,  4129163.65995 };
	waypoint_utm[5]  = { 403188.494255,  4129175.23815 };
	waypoint_utm[6]  = { 403192.579036,  4129177.42123 };
	waypoint_utm[7]  = { 403195.45687,   4129178.70838 };
	waypoint_utm[8]  = { 403196.654117,  4129182.60031 };
	waypoint_utm[9]  = { 403188.253848,  4129179.74587 };
	waypoint_utm[10] = { 403183.344647,  4129175.25328 };
	waypoint_utm[11] = { 403178.839622,  4129171.98767 };
	waypoint_utm[12] = { 403174.786097,  4129167.14121 };
	waypoint_utm[13] = { 403172.102604,  4129165.03072 };
	waypoint_utm[14] = { 403167.683418,  4129166.12481 };

	waypoint_utm_datum = waypoint_utm[0];

	for (int i = 0; i < 15; i++) {
		waypoint_utm_relative[i].x = waypoint_utm[i].x - waypoint_utm_datum.x;
		waypoint_utm_relative[i].y = waypoint_utm[i].y - waypoint_utm_datum.y;
		printf("utm    : %2d번째  X:%6.3lf Y:%6.3lf\n", i+1, waypoint_utm[i].x, waypoint_utm[i].y);
	}
	printf("\n\n\n");
	for (int i = 0; i < 15; i++) {
		printf("utm(r) : %2d번째  X:%6.3lf Y:%6.3lf\n", i + 1, waypoint_utm_relative[i].x, waypoint_utm_relative[i].y);
	}
	printf("\n\n\n");
	utm_link_origin.x   =   0.0;  utm_link_origin.y   =   0.0;  utm_link_origin.yaw = -90.0;

	return 1;
}

void set_rotation_matrix_aixs(double m_angle_degree)
{
	angle_radian = DEG2RAD(m_angle_degree);

	Rotation_matrix_aixs[0][0] = cos(angle_radian);    Rotation_matrix_aixs[0][1] = sin(angle_radian);
	Rotation_matrix_aixs[1][0] = -sin(angle_radian);   Rotation_matrix_aixs[1][1] = cos(angle_radian);
}

void set_rotation_matrix_point(double m_angle_degree)
{
	angle_radian = DEG2RAD(m_angle_degree);

	Rotation_matrix_point[0][0] = cos(angle_radian);   Rotation_matrix_point[0][1] = -sin(angle_radian);
	Rotation_matrix_point[1][0] = sin(angle_radian);   Rotation_matrix_point[1][1] = cos(angle_radian);
}

void TF_utm_map(Point2D utm_point2d, Point2D* map_point2d, Pose2D utm_origin)
{
	set_rotation_matrix_aixs(-utm_origin.yaw); //utm좌표를 90도로 CW방향으로 회전시 map좌표가 됨

	map_point2d->x = (Rotation_matrix_aixs[0][0] * utm_point2d.x) + (Rotation_matrix_aixs[0][1] * utm_point2d.y);
	map_point2d->y = (Rotation_matrix_aixs[1][0] * utm_point2d.x) + (Rotation_matrix_aixs[1][1] * utm_point2d.y);

	printf("utm[%6.3lf,%6.3lf] ----> map[%6.3lf,%6.3lf]\n", utm_point2d.x, utm_point2d.y, map_point2d->x, map_point2d->y);
}

void calculate_waypoint_angle_utm(void) {
	for (int i = 1; i < 15; i++) {
		double dx,dy,angle;
		dx = waypoint_utm_relative[i].x - waypoint_utm_relative[i - 1].x;
		dy = waypoint_utm_relative[i].y - waypoint_utm_relative[i - 1].y;
		angle = atan2( dy, dx);
		waypoint_angle_utm[i] = RAD2DEG(angle);
		printf("wp[%2d] line angle %6.3lf\n", i, RAD2DEG(angle));
	}
}

void calculate_waypoint_angle_map(void) {
	for (int i = 1; i < 15; i++) {
		double dx, dy, angle;
		dx = waypoint_map_relative[i].x - waypoint_map_relative[i - 1].x;
		dy = waypoint_map_relative[i].y - waypoint_map_relative[i - 1].y;
		angle = atan2(dy, dx);
		waypoint_angle_map[i] = RAD2DEG(angle);
		printf("mp[%2d] line angle %6.3lf\n", i, RAD2DEG(angle));
	}
}

void calculate_waypoint_distance(void) {
	for (int i = 1; i < 15; i++) {
		double dx, dy, distance;
		dx = waypoint_map_relative[i].x - waypoint_map_relative[i - 1].x;
		dy = waypoint_map_relative[i].y - waypoint_map_relative[i - 1].y;
		distance = sqrt(powf(dx, 2.0) + powf(dy, 2.0));
		waypoint_distance[i] = distance;
		printf("wp[%2d] line distance %6.3lf[m]\n", i, distance);
	}
}

void calculate_waypoint_equation_utm(void) {
	for (int i = 1; i < 15; i++) {
		double dx, dy, a, b;
		dx = waypoint_utm_relative[i].x - waypoint_utm_relative[i - 1].x;
		dy = waypoint_utm_relative[i].y - waypoint_utm_relative[i - 1].y;
		a = dy / dx;
		b = waypoint_utm_relative[i].y - a * waypoint_utm_relative[i].x;
		waypoint_line_equation_utm[i].a = a;
		waypoint_line_equation_utm[i].b = b;
		printf("wp[%2d] waypoint equation(utm) y = %6.3lf * x + %6.3lf   [m]\n", i, waypoint_line_equation_utm[i].a, waypoint_line_equation_utm[i].b);
	}
}

void calculate_waypoint_equation_map(void) {
	for (int i = 1; i < 15; i++) {
		double dx, dy, a, b;
		dx = waypoint_map_relative[i].x - waypoint_map_relative[i - 1].x;
		dy = waypoint_map_relative[i].y - waypoint_map_relative[i - 1].y;
		a = dy / dx;
		b = waypoint_map_relative[i].y - a * waypoint_map_relative[i].x;
		waypoint_line_equation_map[i].a = a;
		waypoint_line_equation_map[i].b = b;
		printf("wp[%2d] waypoint equation(map) y = %6.3lf * x + %6.3lf   [m]\n", i, waypoint_line_equation_map[i].a, waypoint_line_equation_map[i].b);
	}
}

double calculatte_distance_point_line_equation_map(Point2D in_point2d , int watpoint_id) {
	double distance = 0.0;

	distance = fabs(waypoint_line_equation_map[watpoint_id].a * in_point2d.x + waypoint_line_equation_map[watpoint_id].b - in_point2d.y)
	           / sqrt(waypoint_line_equation_map[watpoint_id].a * waypoint_line_equation_map[watpoint_id].a + 1);
	return distance;
}

double calculatte_distance_point_line_equation_utm(Point2D in_point2d, int watpoint_id) {
	double distance = 0.0;

	distance = fabs(waypoint_line_equation_utm[watpoint_id].a * in_point2d.x + waypoint_line_equation_utm[watpoint_id].b - in_point2d.y)
		/ sqrt(waypoint_line_equation_utm[watpoint_id].a * waypoint_line_equation_utm[watpoint_id].a + 1);
	return distance;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TF_base_link_base_link_map(Point2D base_link_point2d, Point2D* base_link_map_point2d, Pose2D base_link_origin)
{
	set_rotation_matrix_point(base_link_origin.yaw);
	/*printf("\n\n");
	printf("base_link_point      %6.3lf  %6.3lf\n", base_link_point2d->x, base_link_point2d->y);
	printf("base_link_map_point  %6.3lf  %6.3lf\n", base_link_map_point2d->x, base_link_map_point2d->y);
	printf("base_link_origin     %6.3lf  %6.3lf  %6.3lf\n", base_link_origin.x, base_link_origin.y, base_link_origin.theta);*/

	base_link_map_point2d->x = (Rotation_matrix_point[0][0] * base_link_point2d.x) + (Rotation_matrix_point[0][1] * base_link_point2d.y);
	base_link_map_point2d->y = (Rotation_matrix_point[1][0] * base_link_point2d.x) + (Rotation_matrix_point[1][1] * base_link_point2d.y);
}

void TF_base_link_map_map(Point2D base_link_2D, Point2D* base_link_map_2D, Pose2D base_link_origin) {
	base_link_map_2D->x += base_link_origin.x;
	base_link_map_2D->y += base_link_origin.y;
}
void TF_odom_map(Point2D odom_point2d, Point2D* map_point2d, Pose2D odom_orgin) {
	//회전 변환 base_link -> base_link_map
	TF_base_link_base_link_map(odom_point2d, map_point2d, odom_orgin);
	//병행 변환 dase_link_map -> map
	TF_base_link_map_map(odom_point2d, map_point2d, odom_orgin);

	printf("return : %6.3lf  %6.3lf", map_point2d->x, map_point2d->y);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int main(void) {

	double cross_track_error_utm   = 0.0;
	double cross_track_error_map   = 0.0;
	double heading_angle_error     = 0.0;  // heading angle 의 기준은 map의 nort가 0 이며 시계반대 방향(CCW)
	double cross_track_error_angle = 0.0;
	double ctr_factor              = 1.0;
	double target_steering_angle   = 0.0;
	int current_waypoint_id        = 1;    // 현재 목표점 waypoint ID 

	initialize_waypoint();

	for (int i = 0; i < 15; i++) {
		TF_utm_map(waypoint_utm_relative[i], &waypoint_map_relative[i], utm_link_origin);
	}
	calculate_waypoint_angle_utm();
	calculate_waypoint_angle_map();
	calculate_waypoint_distance();
	calculate_waypoint_equation_utm();
	calculate_waypoint_equation_map();

	Point2D test;
	test.x = 1.0;
	test.y = 1.0;

	base_link_origin.x   = 1.0;
	base_link_origin.y   = 1.0;
	base_link_origin.yaw = 45;

	heading_angle_error = - base_link_origin.yaw + waypoint_angle_map[current_waypoint_id];
		
	cross_track_error_utm = calculatte_distance_point_line_equation_utm(test,1);
	cross_track_error_map = calculatte_distance_point_line_equation_map(test, 1);
	cross_track_error_angle = cross_track_error_map * ctr_factor;

	target_steering_angle = heading_angle_error + cross_track_error_angle;

	printf("distance(utm) = %6.3lf\n", cross_track_error_utm);
	printf("distance(map) = %6.3lf\n", cross_track_error_map);
	printf("heading angle error = %6.3lf    cross track error angle = %6.3lf \n", heading_angle_error,cross_track_error_angle);
	printf("steering_angle = %6.3lf \n", target_steering_angle);

	return 0;
}

