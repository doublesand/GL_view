#include "Angel.h"
#include <vector>
#include <string>
#include <fstream>

#pragma comment(lib, "glew32.lib")

using namespace std;

// ��¼������Ƭ�еĶ������еĽṹ��
typedef struct vIndex {
	unsigned int a, b, c;
	vIndex(int ia, int ib, int ic) : a(ia), b(ib), c(ic) {}
} vec3i;

vector<vec3>  vertices;
vector<vec3i> faces;

vector<vec3> points;    //���붥����ɫ���ĵ�
vector<vec3> colors;    //����ƬԪ��ɫ���ĵ�

int nFaces;
int nVertices;
int nEdges;

const vec3 vertex_colors[8] = {
	vec3(1.0, 1.0, 1.0),  // White
	vec3(1.0, 1.0, 0.0),  // Yellow
	vec3(0.0, 1.0, 0.0),  // Green
	vec3(0.0, 1.0, 1.0),  // Cyan
	vec3(1.0, 0.0, 1.0),  // Magenta
	vec3(1.0, 0.0, 0.0),  // Red
	vec3(0.0, 0.0, 0.0),  // Black
	vec3(0.0, 0.0, 1.0)   // Blue
};

GLuint mainWin;                                //������
GLint mLocation;                          //����λ��
int axis = 0;
double theta[3] = {0,0,0};  //�����������ת�Ƕ�
double scale = 1;           //���зŴ���С
vec3 translateTheta(0.0, 0.0, 0.0); //��������ƽ�ƣ�����ֻ��Ҫ�仯xֵ����

//-----------------------------------------------������ʵ��
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//��Ļԭ�������Ͻǣ�����0��0�������ǿ��Ը�����Ļ������һЩ����
		if (x - 300 > 0) { //300����Ļ��ȵ�һ��
			axis = 0;
		}
		else
		{
			axis = 1;
		}
	}
}

void spinCube() {
	theta[axis] += 0.1;
	if (theta[axis] > 360.0) theta[axis] -= 360.0;
	glutPostRedisplay();
}

void diration(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) translateTheta.x += 0.1;
	if (key == GLUT_KEY_RIGHT) translateTheta.x -= 0.1;
	//�����ŵ�������ʱ��ͼ��ɾ���ͼ
	if (key == GLUT_KEY_UP) scale -= 0.1;
	if (key == GLUT_KEY_DOWN) scale += 0.1;
}

void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 't':
		glutIdleFunc(spinCube);
		break;
	case 'p':
		glutIdleFunc(NULL);
		break;
	case 'x':
		axis = 0;
		break;
	case 'y':
		axis = 1;
		break;
	case 'z':
		axis = 2;
		break;
	}
}

void read_off(string filename) {
	if (filename.empty()) return;

	ifstream fin;
	fin.open(filename);
	if (!fin) {
		cout << "�ļ���ȡ����" << endl;
		return;
	}
	//����off�ļ���ʽ��ȡ����
	cout << "�ļ��򿪳ɹ�" << endl;
	//��ȡ�ַ���OFF
	string str;
	fin >> str;
	//��ȡ�ļ��еĶ���������Ƭ��������
	fin >> nVertices >> nFaces >> nEdges;
	// ���ݶ�������ѭ����ȡÿ����������
	for (int i = 0; i < nVertices; i++) {
		vec3 node;  
		fin >> node.x >> node.y >> node.z;  //ע��������С��
		vertices.push_back(node);
	}
	// ������Ƭ����ѭ����ȡÿ����Ƭ��Ϣ�����ù�����vec3i�ṹ�屣��
	for (int i = 0; i < nFaces; i++) {
		int num, a, b, c;
		fin >> num >> a >> b >> c;
		faces.push_back(vec3i(a, b, c));
	}
	fin.close();
}

void storeFacesPoints()
{
	points.clear();
	colors.clear();
	// ��points��colors�����д洢ÿ��������Ƭ�ĸ��������ɫ��Ϣ
	for (int i = 0; i < nFaces; i++)
	{
		// ��points�����У��������ÿ����Ƭ�Ķ��㣬����colors�����У���Ӹõ����ɫ��Ϣ
		points.push_back(vertices[faces[i].a]);
		colors.push_back(vertex_colors[i/2]);

		points.push_back(vertices[faces[i].b]);
		colors.push_back(vertex_colors[i/2]);

		points.push_back(vertices[faces[i].c]);
		colors.push_back(vertex_colors[i/2]);
	}
}

void init() {
	//��OFF�ļ��е�����ת�ɹ�������
	storeFacesPoints();  
	
	//���������������
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//��������ʼ�����㻺�����
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3) + colors.size()*sizeof(vec3), NULL, GL_STATIC_DRAW);

	//��ȡ�������ɫ����������ݵ��ڴ���
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec3), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), colors.size() * sizeof(vec3), &colors[0]);
	
	//��ȡ��ɫ����ʹ��
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	//�Ӷ�����ɫ���г�ʼ�������λ��
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//��ƬԪ��ɫ���г�ʼ���������ɫ
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec3)));

	// ��þ���洢λ��
	mLocation = glGetUniformLocation(program, "rotation");  

	//��ɫ����
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display() {   //������õ�ƬԪ��ɫ��
	//�����ڣ�������ɫ�������Ȼ���
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//����һ����λ���󣬹�����
	mat4 m(1.0, 0.0, 0.0, 0.0,
		   0.0, 1.0, 0.0, 0.0,
		   0.0, 0.0, 1.0, 0.0,
		   0.0, 0.0, 0.0, 1.0);

	// ���ú����������ֱ仯�ı仯��������仯����
	m = RotateX(theta[0])*RotateY(theta[1])*RotateZ(theta[2]) * Scale(scale) * Translate(translateTheta)* m;
	glUniformMatrix4fv(mLocation, 1, GL_TRUE, m);

	//���ƶ���
	glDrawArrays(GL_TRIANGLES, 0, points.size()); //ÿ������Ϊһ�������Σ�����������
	glutSwapBuffers();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);

	//������ʾģʽ֧����Ȳ���
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("��ȡoff�ļ�����3Dģ��");

	//ϣ���ڵ����κ�OpenGL����ǰ��ʼ��GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	//��ȡoff�ļ�
	read_off("cube.off");

	init();
	glutDisplayFunc(display);
	glutIdleFunc(spinCube);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(diration);

	//������Ȳ���
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
	return 0;
}