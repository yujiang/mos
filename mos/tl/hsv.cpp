#include "math.h"
#include <windows.h>
#pragma warning(disable:4244)

typedef float Float;
#define DeclspecAlign(size) __declspec(align(size))
#undef PI
#define PI (3.1415926535897932)
#define SMALL_NUMBER (1.e-8)

// 平方根的倒数
inline Float QInvSqrt( Float F )
{
	const Float fThree = 3.0f;
	const Float fOneHalf = 0.5f;
	Float temp;

	__asm
	{
		movss	xmm1,[F]
		rsqrtss	xmm0,xmm1			

			movss	xmm3,[fThree]
		movss	xmm2,xmm0
			mulss	xmm0,xmm1			
			mulss	xmm0,xmm2			
			mulss	xmm2,[fOneHalf]		
		subss	xmm3,xmm0			
			mulss	xmm3,xmm2			
			movss	[temp],xmm3
	}

	return temp;
}

class QVector
{
public:
	Float X, Y, Z;
	QVector(){}

	QVector(Float x, Float y, Float z)
		:X(x),Y(y),Z(z){}
	QVector( const class QVector4& V );

	// 单位化
	bool Normalize(Float tolerance=SMALL_NUMBER)
	{
		const Float squreSum = X*X + Y*Y + Z*Z;
		if(squreSum == 1.0f)
			return TRUE;
		else if (squreSum >= tolerance)
		{
			const Float scale = QInvSqrt(squreSum);
			X *= scale;
			Y *= scale;
			Z *= scale;
			return TRUE;
		}

		return FALSE;
	}
};

DeclspecAlign(16) class QVector4
{
public:
	Float X, Y, Z, W;
	QVector4(Float x = 0.0f, Float y = 0.0f, Float z = 0.0f, Float w = 0.0f)
		:X(x),Y(y),Z(z),W(w){}
};


class QPlane : public QVector
{
public:
	// vector 记录面的法向量
	// W 记录到原点的距离
	Float W;

	QPlane(Float x, Float y, Float z, Float w)
		:QVector(x,y,z),W(w){}

	// 面的法向量和到原点的距离
	QPlane(QVector normal, Float w)
		:QVector(normal), W(w){}
};

class QMatrix
{
public:
	union 
	{
		DeclspecAlign(16) Float M[4][4];
	};
	DeclspecAlign(16) static const QMatrix Identity;
	QMatrix(){}
	QMatrix(const QPlane& inX, const QPlane& inY, const QPlane& inZ, const QPlane& inW)
	{
		M[0][0] = inX.X; M[0][1] = inX.Y; M[0][2] = inX.Z; M[0][3] = inX.W;
		M[1][0] = inY.X; M[1][1] = inY.Y; M[1][2] = inY.Z; M[1][3] = inY.W;
		M[2][0] = inZ.X; M[2][1] = inZ.Y; M[2][2] = inZ.Z; M[2][3] = inZ.W;
		M[3][0] = inW.X; M[3][1] = inW.Y; M[3][2] = inW.Z; M[3][3] = inW.W;
	}

	QMatrix operator*(const QMatrix& Other) const
	{
		QMatrix Result;
		Result.M[0][0] = M[0][0] * Other.M[0][0] + M[0][1] * Other.M[1][0] + M[0][2] * Other.M[2][0] + M[0][3] * Other.M[3][0];
		Result.M[0][1] = M[0][0] * Other.M[0][1] + M[0][1] * Other.M[1][1] + M[0][2] * Other.M[2][1] + M[0][3] * Other.M[3][1];
		Result.M[0][2] = M[0][0] * Other.M[0][2] + M[0][1] * Other.M[1][2] + M[0][2] * Other.M[2][2] + M[0][3] * Other.M[3][2];
		Result.M[0][3] = M[0][0] * Other.M[0][3] + M[0][1] * Other.M[1][3] + M[0][2] * Other.M[2][3] + M[0][3] * Other.M[3][3];

		Result.M[1][0] = M[1][0] * Other.M[0][0] + M[1][1] * Other.M[1][0] + M[1][2] * Other.M[2][0] + M[1][3] * Other.M[3][0];
		Result.M[1][1] = M[1][0] * Other.M[0][1] + M[1][1] * Other.M[1][1] + M[1][2] * Other.M[2][1] + M[1][3] * Other.M[3][1];
		Result.M[1][2] = M[1][0] * Other.M[0][2] + M[1][1] * Other.M[1][2] + M[1][2] * Other.M[2][2] + M[1][3] * Other.M[3][2];
		Result.M[1][3] = M[1][0] * Other.M[0][3] + M[1][1] * Other.M[1][3] + M[1][2] * Other.M[2][3] + M[1][3] * Other.M[3][3];

		Result.M[2][0] = M[2][0] * Other.M[0][0] + M[2][1] * Other.M[1][0] + M[2][2] * Other.M[2][0] + M[2][3] * Other.M[3][0];
		Result.M[2][1] = M[2][0] * Other.M[0][1] + M[2][1] * Other.M[1][1] + M[2][2] * Other.M[2][1] + M[2][3] * Other.M[3][1];
		Result.M[2][2] = M[2][0] * Other.M[0][2] + M[2][1] * Other.M[1][2] + M[2][2] * Other.M[2][2] + M[2][3] * Other.M[3][2];
		Result.M[2][3] = M[2][0] * Other.M[0][3] + M[2][1] * Other.M[1][3] + M[2][2] * Other.M[2][3] + M[2][3] * Other.M[3][3];

		Result.M[3][0] = M[3][0] * Other.M[0][0] + M[3][1] * Other.M[1][0] + M[3][2] * Other.M[2][0] + M[3][3] * Other.M[3][0];
		Result.M[3][1] = M[3][0] * Other.M[0][1] + M[3][1] * Other.M[1][1] + M[3][2] * Other.M[2][1] + M[3][3] * Other.M[3][1];
		Result.M[3][2] = M[3][0] * Other.M[0][2] + M[3][1] * Other.M[1][2] + M[3][2] * Other.M[2][2] + M[3][3] * Other.M[3][2];
		Result.M[3][3] = M[3][0] * Other.M[0][3] + M[3][1] * Other.M[1][3] + M[3][2] * Other.M[2][3] + M[3][3] * Other.M[3][3];
		return Result;

	}
	// QVector4 * QMatrix
	QVector4 TransformVector4(const QVector4& v) const
	{
		QVector4 result;
		result.X = v.X * M[0][0] + v.Y * M[1][0] + v.Z * M[2][0] + v.W * M[3][0];
		result.Y = v.X * M[0][1] + v.Y * M[1][1] + v.Z * M[2][1] + v.W * M[3][1];
		result.Z = v.X * M[0][2] + v.Y * M[1][2] + v.Z * M[2][2] + v.W * M[3][2];
		result.W = v.X * M[0][3] + v.Y * M[1][3] + v.Z * M[2][3] + v.W * M[3][3];
		return result;
	}
};


QMatrix GetScaleMat(float s) ;

QMatrix GetTranslateMat(QVector t) ;

QMatrix GetRotateMat(QVector d, float ang);


QVector::QVector( const QVector4& v )
	:X(v.X),Y(v.Y),Z(v.Z){}

QMatrix GetScaleMat(float s) 
{
	return QMatrix
		(
		QPlane(s,	0.f, 0.f, 0.f),
		QPlane(0.f, s,	 0.f, 0.f),
		QPlane(0.f, 0.f, s,	  0.f),
		QPlane(0.f, 0.f, 0.f, 1.f)
		);
}

QMatrix GetTranslateMat(QVector t) 
{
	return QMatrix
		(
		QPlane(1.f, 0.f, 0.f, 0.f),
		QPlane(0.f, 1.f, 0.f, 0.f),
		QPlane(0.f, 0.f, 1.f, 0.f),
		QPlane(t, 1.f)
		);
}

QMatrix GetRotateMat(QVector d, float ang) 
{
	float s = sinf(ang);
	float c = cosf(ang);
	d.Normalize();
	return QMatrix
		(
		QPlane(d.X*d.X*(1-c)+c, d.X*d.Y*(1-c)-d.Z*s, d.X*d.Z*(1-c)+d.Y*s, 0.f),
		QPlane(d.X*d.Y*(1-c)+d.Z*s, d.Y*d.Y*(1-c)+c, d.Y*d.Z*(1-c)-d.X*s, 0.f),
		QPlane(d.X*d.Z*(1-c)-d.Y*s, d.Y*d.Z*(1-c)+d.X*s, d.Z*d.Z*(1-c)+c, 0.f),
		QPlane(0.f, 0.f, 0.f, 1.f)
		);
}


QMatrix* GetMat(float h,float s,float v)
{
	float hue = -(float)h;

	// -5 - 5
	float saturation = ((float)s+100.f)/100.f;
	//float saturation = 5.0f;

	// 0 - 5
	float brightness = ((float)v+20.f)/20.f;

	// -5 - 5
	float contrast = 1.0f;

	QMatrix brightnessMat = GetScaleMat(brightness);
	QMatrix contrastMat = GetTranslateMat(QVector(-0.5f, -0.5f, -0.5f));
	contrastMat = contrastMat*GetScaleMat(contrast);
	contrastMat = contrastMat*GetTranslateMat(QVector(0.5f, 0.5f, 0.5f));

	const float rwgt = 0.3086f;
	const float gwgt = 0.6094f;
	const float bwgt = 0.0820f;
	float reSaturation = 1 - saturation;
	QMatrix saturationMat = QMatrix
		(
		QPlane(reSaturation*rwgt+saturation,	reSaturation*rwgt,				reSaturation*rwgt,				0.f),
		QPlane(reSaturation*gwgt,				reSaturation*gwgt+saturation,	reSaturation*gwgt,				0.f),
		QPlane(reSaturation*bwgt,				reSaturation*bwgt,				reSaturation*bwgt+saturation,	0.f),
		QPlane(0.f,								0.f,							0.f,							1.f)
		);

	QMatrix hueMat = GetRotateMat(QVector(1.f,1.f,1.f), hue/360.f * 2 * PI);

	static QMatrix colorMatrix;
	colorMatrix = brightnessMat * contrastMat * saturationMat * hueMat;
	return &colorMatrix;
}

#pragma pack (push)
#pragma pack (1)

struct WPixel {
	union {
		WORD color;								// 16bit 高彩
		struct {
			WORD blue:5;						// 5bit 兰色
			WORD green:6;						// 6bit 绿色
			WORD red:5;							// 5bit 红色
		};
	};
	WPixel() {}
	WPixel(DWORD c) {color=(WORD)((c>>8&0xf800)|(c>>5&0x7e0)|(c>>3&0x1f));}
	WPixel(int r,int g,int b) : red(r>>3),green(g>>2),blue(b>>3) {}
	operator DWORD() const {return (color<<5&0x7fc00)|(color<<8&0xf80000)|((color<<3|(color&7))&0x3ff);}
	void SetRGB(int r, int g, int b){red=r>>3;green=g>>2;blue=b>>3;}
};

struct WPixelRGB{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	WPixelRGB(){}
	WPixelRGB(int r,int g,int b) : red(r),green(g),blue(b) {}
};

#pragma pack (pop)

WORD RGBChange(WPixel& c,const QMatrix& matrix)
{
	DWORD color = (DWORD)(c);
	float rr, gg, bb;
	rr = color>>16&0xff;
	gg = color>>8&0xff;
	bb = color&0xff;
	QVector4 vec(rr/255,gg/255,bb/255,1.0f);
	QVector4 new_vec =matrix.TransformVector4(vec);
	new_vec.X = max((min(1.0,new_vec.X)),0.0f);
	new_vec.Y = max((min(1.0,new_vec.Y)),0.0f);
	new_vec.Z = max((min(1.0,new_vec.Z)),0.0f);
	new_vec.W = max((min(1.0,new_vec.W)),0.0f);

	WPixel _pixel;
	_pixel.SetRGB(new_vec.X*255,new_vec.Y*255,new_vec.Z*255);
	return _pixel.color;
}

WORD* create_pal_hsv(WORD* pal,int h,int s,int v)
{
	WORD* p = new WORD[256];

	QMatrix* matrix = GetMat(h, s, v);
	WORD* src = pal;
	WORD* des = p;
	for(int j=0;j<256;j++,src++,des++){
		WPixel c;
		c.color = *src;
		*des = RGBChange(c, *matrix);
	}

	return p;
}
