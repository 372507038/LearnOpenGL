#version 330

uniform mat4 model_matrix;
uniform mat4 projection_matrix;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

// out vec4 vs_ps_clr; // ����ԭ��,ps z�е�����������ƣ�Ҫ��vs�е������������һ�£��������ȡ���������޷�����
   out vec4 vs_fs_color;
void main()
{
	//vs_ps_clr = color;

	gl_Position = projection_matrix * (model_matrix * position);
	vs_fs_color = color; // vs_fs_color ��vs����out����

}