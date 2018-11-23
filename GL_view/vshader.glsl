#version 330 core

in vec3 vPosition;  //�����������ݲ��ܸı䣬���Թ���
in vec3 vColor;
out vec3 color;
uniform mat4 modelViewMatrix;

void main()
{
	//���һ������ʵ�����š���ת��ƽ�Ʊ任 
    gl_Position = modelViewMatrix * vec4(0.5 * vPosition, 1.0);  //gl_Position�����ö���������ᴫ����դ��ģ����
    color = vColor;
}
