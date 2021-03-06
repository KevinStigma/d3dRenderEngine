#ifndef ZYK_ARCBALL_H
#define ZYK_ARCBALL_H
#include "ArcBall.h"
class MyArcball
{
public:
	MyArcball(float w, float h) :ArcBall(NULL)
	{
		ArcBall=new ArcBallT(w,h);
		init();
	}
	~MyArcball()
	{
		if (ArcBall)
		{
			delete ArcBall;
			ArcBall = NULL;
		}
	}
	ArcBallT*    ArcBall;
	Matrix4fT   Transform;
	Matrix3fT   LastRot;
	Matrix3fT   ThisRot;
	Point2fT    MousePt;  
	Point2fT	mLastMousePos;
	int			button_status;
	float translate_x,translate_y;
	void init()
	{
		button_status=translate_x=translate_y=0;
		Matrix3fSetIdentity(&LastRot); 
		Matrix3fSetIdentity(&ThisRot); 
		Transform.M[0]=1.0f;Transform.M[1]=0.0f;Transform.M[2]=0.0f;Transform.M[3]=0.0f;
		Transform.M[4]=0.0f;Transform.M[5]=1.0f;Transform.M[6]=0.0f;Transform.M[7]=0.0f;
		Transform.M[8]=0.0f;Transform.M[9]=0.0f;Transform.M[10]=1.0f;Transform.M[11]=0.0f;
		Transform.M[12]=0.0f;Transform.M[13]=0.0f;Transform.M[14]=0.0f;Transform.M[15]=1.0f;
	}
	void updateScreenSize(int width,int height)
	{
		if (ArcBall)
		{
			delete ArcBall;
			ArcBall = NULL;
		}
		ArcBall = new ArcBallT(width, height);
	}
};
#endif