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

bool textureMode = true;
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

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

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
		s.scale = s.scale*0.08;
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
			c.scale = c.scale*1.5;
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

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
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

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
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

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
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


double* getNormal(double* start, double* end1, double* end2) {
	const double ax = end1[0] - start[0];
	const double bx = end2[0] - start[0];
	const double ay = end1[1] - start[1];
	const double by = end2[1] - start[1];
	const double az = end1[2] - start[2];
	const double bz = end2[2] - start[2];

	double nx = ay * bz - by * az;
	double ny = -ax * bz + bx * az;
	double nz = ax * by - bx * ay;

	double length = sqrt(nx * nx + ny * ny + nz * nz);
	nx = nx / length;
	ny = ny / length;
	nz = nz / length;
	double* normal = new double[3]{ nx,ny,nz };
	return normal;
}

double Q1[] = { 8, 7, 0 };
double Q2[] = { 0, 10, 0 };
double Q3[] = { 6, 16, 0 };
double Q4[] = { 9, 10, 0 };
double Q5[] = { 15, 13, 0 };
double Q6[] = { 11, 9, 0 };
double Q7[] = { 16, 5, 0 };
double Q8[] = { 9, 0, 0 };

double Q11[] = { 8, 7, 10 };
double Q12[] = { 0, 10, 10 };
double Q13[] = { 6, 16, 10 };
double Q14[] = { 9, 10, 10 };
double Q15[] = { 15, 13, 10 };
double Q16[] = { 11, 9, 10 };
double Q17[] = { 16, 5, 10 };
double Q18[] = { 9, 0, 10 };

const double xm = (Q2[0] + Q3[0]) / 2;
const double ym = (Q2[1] + Q3[1]) / 2;
const double atanAngle = atan((Q3[1] - Q2[1]) / (Q3[0] - Q2[0]));
const double radius = sqrt(pow(xm - Q2[0], 2) + pow(ym - Q2[1], 2));

void polygon() {
	glBegin(GL_TRIANGLE_FAN);
	glVertex3dv(Q1);
	glVertex3dv(Q2);
	glVertex3dv(Q3);
	glVertex3dv(Q4);
	glVertex3dv(Q5);
	glVertex3dv(Q6);
	glVertex3dv(Q7);
	glVertex3dv(Q8);
	glEnd();


	// ����������
	glBegin(GL_TRIANGLE_FAN);

	glVertex3d(xm, ym, 0);

	for (int i = 0; i <= 180; i++) {
		const double x = xm + cos(i * PI / 180 + atanAngle) * radius;
		const double y = ym + sin(i * PI / 180 + atanAngle) * radius;
		glVertex3d(x, y, 0);
	}
	glEnd();
}

void prism() {

	double* normal;

	glColor3d(0.3, 0.5, 0.5);
	glNormal3d(0, 0, -1);
	polygon();

	glPushMatrix();
	glTranslated(0, 0, 10);
	glColor3d(0.7, 0.9, 0.9);
	glNormal3d(0, 0, 1);
	polygon();
	glPopMatrix();

	// �������
	glColor3d(0.5, 0.7, 0.7);
	glBegin(GL_QUAD_STRIP);
	for (int i = 1; i <= 180; i++) {
		const double xprev = xm+cos((i-1) * PI / 180 + atanAngle) * radius;
		const double yprev = ym+sin((i-1) * PI / 180 + atanAngle) * radius;

		const double x = xm+cos(i * PI / 180 + atanAngle) * radius;
		const double y = ym+sin(i * PI / 180 + atanAngle) * radius;
		glNormal3d(x-xm,y-ym, 0);
		glVertex3f(xprev, yprev, 10);
		glVertex3f(xprev, yprev, 0);
		glVertex3f(x, y, 10);
		glVertex3f(x, y, 0);
	}
	glEnd();

	glBegin(GL_QUADS);
	normal = getNormal(Q2, Q1, Q12);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(Q2);
	glVertex3dv(Q12);
	glVertex3dv(Q11);
	glVertex3dv(Q1);
	normal = getNormal(Q1, Q8, Q11);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(Q1);
	glVertex3dv(Q11);
	glVertex3dv(Q18);
	glVertex3dv(Q8);
	normal = getNormal(Q8, Q7, Q18);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(Q8);
	glVertex3dv(Q18);
	glVertex3dv(Q17);
	glVertex3dv(Q7);
	normal = getNormal(Q7, Q6, Q17);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(Q7);
	glVertex3dv(Q17);
	glVertex3dv(Q16);
	glVertex3dv(Q6);
	normal = getNormal(Q6, Q5, Q16);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(Q6);
	glVertex3dv(Q16);
	glVertex3dv(Q15);
	glVertex3dv(Q5);
	normal = getNormal(Q5, Q4, Q15);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(Q5);
	glVertex3dv(Q15);
	glVertex3dv(Q14);
	glVertex3dv(Q4);
	normal = getNormal(Q4, Q3, Q14);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(Q4);
	glVertex3dv(Q14);
	glVertex3dv(Q13);
	glVertex3dv(Q3);
	
	

	glEnd();

}




void Render(OpenGL *ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1 };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1 };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1};
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	prism();

	

	//������ ��������� ���������� ��������
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/

	//����� ��������� ���������� ��������


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
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}