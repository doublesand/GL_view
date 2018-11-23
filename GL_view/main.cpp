#include "Angel.h"
#include <vector>
#include <string>
#include <fstream>

#pragma comment(lib, "glew32.lib")

using namespace std;

// �������
float radius = 1.0;       //���������С
float rotateAngle = 0.0;  //����������ת
float upAngle = 0.0;      //����������ת

// ͸��ͶӰ����
float fov = 45.0;
float aspect = 1.0;

// ����ͶӰ����
float scale = 1.5;

//ͶӰѡ�����
int matrixIndex = 0;

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


//-----------------------------------------------������ʵ��
void idle(void)
{
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		matrixIndex = 1;
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
		matrixIndex = 0;
	}
}

void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 033:	// ESC�� �� 'q' ���˳�
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit(EXIT_SUCCESS);
		break;
		// �����ı�radius, rotateAngle, upAngle����
	case 'a':
		radius += 0.1;
		break;
	case 's':
		radius -= 0.1;
		break;
	case 'd':
		rotateAngle += 0.1;
		break;
	case 'f':
		rotateAngle -= 0.1;
		break;
	case 'g':
		upAngle += 0.1;
		break;
	case 'h':
		upAngle -= 0.1;
	}
	glutPostRedisplay();
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
	mLocation = glGetUniformLocation(program, "modelViewMatrix");  

	//��ɫ����
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display() {   //������õ�ƬԪ��ɫ��
	//�����ڣ�������ɫ�������Ȼ���
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//�������������ͨ���������Ʋ����ı䣬�仯���λ�ã���eye������Ĭ��Ϊ��1��0��0��
	float x = radius * cos(upAngle);
	float y = radius * sin(upAngle) * cos(rotateAngle);
	float z = radius * sin(upAngle) * sin(rotateAngle);

	vec4 eye = vec4(x, y, z, 1);  //�����۾���λ��
	vec4 at =  vec4(0, 0, 0, 1);  //ģ�͵�����
	vec4 up =  vec4(0, 1, 0, 0);  //���ϵ�λ��

	//����һ����λ����
	mat4 modelMatrix = mat4(1.0);
	mat4 viewMatrix = LookAt(eye, at, up);
	//��ʵͶӰ����������ɽ������ͼ��Ŵ����С��ʾ���Ӽ�����
	mat4 projMatrix = Ortho(-scale, scale, -scale, scale, -scale, scale);
	if (matrixIndex == 0) {  //����ͶӰ�任
		projMatrix = Perspective(fov, aspect, 0.1, 100.0);
	}
	// �������յ�ͶӰ�任���󡪡�ͶӰ����*�������*ģ�;���
	mat4 modelViewMatrix = projMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(mLocation, 1, GL_TRUE, modelViewMatrix);

	//���ƶ���
	glDrawArrays(GL_TRIANGLES, 0, points.size()); //ÿ������Ϊһ�������Σ�����������
	glutSwapBuffers();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);

	//������ʾģʽ֧����Ȼ���
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
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	//������Ȳ���
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);  //���޳���Ĭ��ȥ�������в������泯����棬��ʱ����������Ȳ���
	glutMainLoop();
	return 0;
}