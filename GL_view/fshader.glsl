#version 330 core

in vec3 color;

void main()
{
    gl_FragColor = vec4(color, 1.0);  //gl_FragColor��������ɫ�������Ὣ�䴫����դ�����������
}
