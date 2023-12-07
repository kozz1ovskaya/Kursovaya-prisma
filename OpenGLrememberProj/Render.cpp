#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = false;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

struct Vector {
	float x;
	float y;
	float z;
};

///!!!!!!!!!!
Vector FindNormal(const double(&p1)[3], const double(&p2)[3], const double(&p3)[3])
{
	Vector v1, v2, normal;

	// ��������� ������� ������������
	v1.x = p2[0] - p1[0];
	v1.y = p2[1] - p1[1];
	v1.z = p2[2] - p1[2];

	v2.x = p3[0] - p1[0];
	v2.y = p3[1] - p1[1];
	v2.z = p3[2] - p1[2];

	// ��������� ��������� ������������
	normal.x = v1.y * v2.z - v1.z * v2.y;
	normal.y = -(v1.x * v2.z - v1.z * v2.x);
	normal.z = v1.x * v2.y - v1.y * v2.x;

	// ����������� ������
	float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
	normal.x /= length;
	normal.y /= length;
	normal.z /= length;
	glNormal3d(normal.x, normal.y, normal.z);
	return normal;
}

void MakePrism()
{
	double h = 15;

	double b = 0.01;
	double a = 0;
	double r2 = sqrt(40) / 2;
	double radius = sqrt(40) / 2;

	double A[] = { 1,0,0 };
	double Ah[] = { 1,0,15 };

	double B[] = { 5,7,0 };
	double Bh[] = { 5,7,15 };

	double C[] = { -1,9,0 };
	double Ch[] = { -1,9,15 };

	double D[] = { -1,1,0 };
	double Dh[] = { -1,1,15 };

	double E[] = { -8,1,0 };
	double Eh[] = { -8,1,15 };

	double F[] = { -2,-1,0 };
	double Fh[] = { -2,-1,15 };

	double G[] = { -2,-7,0 };
	double Gh[] = { -2,-7,15 };

	double H[] = { 5,-6,0 };
	double Hh[] = { 5,-6,15 };

	double P_center[] = { 2,8,0 };
	double P_centerh[] = { 2,8,15 };
	double dy = B[1] - C[1];
	double dx = B[0] - C[0];
	double deg2 = atan2(dy, dx) * 180 / M_PI + 306.86;
	double deg = atan2(dy, dx) * 180 / M_PI;
	double angle_deg = atan2(dy, dx) * 180 / M_PI;
	double p1[] = { 0,0,0 };
	double p2[] = { 0,0,0 };
	double p3[] = { 0,0,0 };


	FindNormal(A, B, C);
	glBegin(GL_TRIANGLES);
	glColor3d(0.5, 0.6, 0.8);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	glVertex3dv(A);
	glVertex3dv(C);
	glVertex3dv(D);

	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(F);

	glVertex3dv(A);
	glVertex3dv(D);
	glVertex3dv(F);

	glVertex3dv(A);
	glVertex3dv(F);
	glVertex3dv(G);

	glVertex3dv(A);
	glVertex3dv(G);
	glVertex3dv(H);
	glEnd();

	FindNormal(Ah, Bh, Ch);
	glBegin(GL_TRIANGLES);
	glColor3d(0.5, 0.6, 0.8);
	glVertex3dv(Ah);
	glVertex3dv(Bh);
	glVertex3dv(Ch);

	glVertex3dv(Ah);
	glVertex3dv(Ch);
	glVertex3dv(Dh);

	glVertex3dv(Dh);
	glVertex3dv(Eh);
	glVertex3dv(Fh);

	glVertex3dv(Ah);
	glVertex3dv(Dh);
	glVertex3dv(Fh);

	glVertex3dv(Ah);
	glVertex3dv(Fh);
	glVertex3dv(Gh);

	glVertex3dv(Ah);
	glVertex3dv(Gh);
	glVertex3dv(Hh);
	glEnd();


	FindNormal(A, B, Bh);
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.2, 0.0);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(Bh);
	glVertex3dv(Ah);
	glEnd();


	/*FindNormal(B, C, Ch);
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.2, 0.0);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(Ch);
	glVertex3dv(Bh);
	glEnd();*/


	FindNormal(C, D, Ch);
	glBegin(GL_POLYGON);
	glColor3d(0.5, 0.2, 0.0);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(Dh);
	glVertex3dv(Ch);
	glEnd();


	FindNormal(D, E, Eh);
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.2, 0.0);
	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(Eh);
	glVertex3dv(Dh);
	glEnd();

	FindNormal(E, F, Fh);
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.2, 0.0);
	glVertex3dv(E);
	glVertex3dv(F);
	glVertex3dv(Fh);
	glVertex3dv(Eh);
	glEnd();

	FindNormal(F, G, Gh);
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.2, 0.0);
	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(Gh);
	glVertex3dv(Fh);
	glEnd();


	FindNormal(G, H, Hh);
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.2, 0.0);
	glVertex3dv(G);
	glVertex3dv(H);
	glVertex3dv(Hh);
	glVertex3dv(Gh);
	glEnd();

	FindNormal(H, A, Ah);
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.2, 0.0);
	glVertex3dv(H);
	glVertex3dv(A);
	glVertex3dv(Ah);
	glVertex3dv(Hh);
	glEnd();

	p1[0] = 1.115;
	p1[1] = -1.115;
	p1[2] = 0;
	p2[0] = 1.115;
	p2[1] = 1.115;
	p2[2] = 15;

	glTranslated(P_center[0], P_center[1], P_center[2]);	
	for (a = deg2; a < deg2 + 180;) 
	{
		p1[0] = 0;
		p1[1] = 0;
		p1[2] = 0;
		p2[0] = r2 * sin(a * PI / 180);
		p2[1] = r2 * cos(a * PI / 180);
		p2[2] = 0;
		p3[0] = r2 * sin((a + b) * PI / 180);
		p3[1] = r2 * cos((a + b) * PI / 180);
		p3[2] = 0;
		FindNormal(p2, p1, p3);
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.5, 0.1, 0.5);
		glVertex3d(0, 0, 0);
		glVertex3d(r2 * sin(a * PI / 180), r2 * cos(a * PI / 180), 0); 
		glVertex3d(r2 * sin((a + b) * PI / 180), r2 * cos((a + b) * PI / 180), 0);
		glEnd();
		p1[2] = h;
		p2[2] = h;
		p3[2] = h;

		FindNormal(p2, p1, p3);
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.5, 0.1, 0.5);
		glVertex3d(0, 0, 15);
		glVertex3d(r2 * sin(a * PI / 180), r2 * cos(a * PI / 180), h);
		glVertex3d(r2 * sin((a + b) * PI / 180), r2 * cos((a + b) * PI / 180), h);
		glEnd();

		p1[0] = r2 * sin(a * PI / 180);
		p1[1] = r2 * cos(a * PI / 180);
		p1[2] = h;
		p2[0] = r2 * sin((a + b) * PI / 180);
		p2[1] = r2 * cos((a + b) * PI / 180);
		p2[2] = h;
		p3[0] = r2 * sin((a + b) * PI / 180);
		p3[1] = r2 * cos((a + b) * PI / 180);
		p3[2] = 0;
		FindNormal(p1, p2, p3);
		glBegin(GL_POLYGON);
		glVertex3d(r2 * sin(a * PI / 180), r2 * cos(a * PI / 180), 15);
		glVertex3d(r2 * sin((a + b) * PI / 180), r2 * cos((a + b) * PI / 180), h);
		glVertex3d(r2 * sin((a + b) * PI / 180), r2 * cos((a + b) * PI / 180), 0);
		glVertex3d(r2 * sin(a * PI / 180), r2 * cos(a * PI / 180), 0);
		glEnd();
		a += b;
	}
}

void Render(OpenGL* ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);

	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//���� �������� ��������� ���������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//���� ����������� ��������� ���������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//���� ����������� ��������� ���������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������� � ������� ����������� ��������� (����������� ������). ����������� �������� � ��������� [0; 128].
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	MakePrism();

	//===================================
   //��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}