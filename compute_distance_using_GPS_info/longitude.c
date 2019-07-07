#include <stdio.h>
#include <math.h>

typedef unsigned int   UINT32;
typedef signed int     SINT32;
typedef unsigned short UINT16;

enum LATI_SIGN
{
    north = 0,
    sorth
};

typedef struct GPS_STUB_INFO_ST
{
    UINT32 degrees_latitude; //角度！
    SINT32 degrees_longitude;
    UINT16 latitude_sign;   //north=0 sorth=1
    UINT16 altitude_direction;
    UINT16 altitude;
}GPS_STUB_INFO;

typedef struct TDPointSt
{
    double va_x;
    double va_y;
    double va_z;
    //double va_rh;
}tDPoint;

#define EARTY_RADIUS 6371393  //m
#define PI 3.1415926

void gpsInfoToPoint(GPS_STUB_INFO * infoA, tDPoint * pointA)
{
    double radW = 0.0;
    double radJ = 0.0;
    double equ_altitude = 0.0;

    radW = ((-2.0)*(infoA->latitude_sign) + 1)*((infoA->degrees_latitude)/10000000.0)*PI/180.0;
    radJ = ((infoA->degrees_longitude)/10000000.0)*PI/180.0;
    equ_altitude = EARTY_RADIUS + (2*(infoA->altitude_direction)-1)*(infoA->altitude)/1.0;

    pointA->va_x = cos(radW)*cos(radJ)*equ_altitude;
    pointA->va_y = cos(radW)*sin(radJ)*equ_altitude;
    pointA->va_z = sin(radW)*equ_altitude;
}


double xyzToDistance(tDPoint * pointA, tDPoint * pointB )
{
    double x,y,z,d;
    x = fabs(pointA->va_x - pointB->va_x);
    y = fabs(pointA->va_y - pointB->va_y);
    z = fabs(pointA->va_z - pointB->va_z);
    d = sqrt(x*x + y*y + z*z);

    return d;
}


int main(int argc, char *argv[])
{
    /*if(argc != 3)
    {
        printf("Need 3 param!\n");
        return -1;
    }*/
    GPS_STUB_INFO a1, a2;
    tDPoint b1, b2;
    double distance = 0.0;

    memset(&a1, 0x0, sizeof(GPS_STUB_INFO));
    memset(&a2, 0x0, sizeof(GPS_STUB_INFO));

    memset(&b1, 0x0, sizeof(tDPoint));
    memset(&b2, 0x0, sizeof(tDPoint));

    //data input
    a1.degrees_latitude = 312444403;
    a1.latitude_sign = 0;
    a1.degrees_longitude = -1215930728;
    a1.altitude_direction = 1;
    a1.altitude = 23;

    a2.degrees_latitude = 312441063;
    a2.latitude_sign = 0;
    a2.degrees_longitude = -1215928335;
    a2.altitude_direction = 1;
    a2.altitude = 81;

    gpsInfoToPoint(&a1, &b1);
    gpsInfoToPoint(&a2, &b2);

    distance = xyzToDistance(&b1, &b2);
    printf("distance = %.1fm\n", distance);

    return 0;
}
