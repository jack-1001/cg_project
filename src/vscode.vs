"#version 330 core
"
"in vec3 vColorOut;  // ������ɫ�����ݵ���ɫ
"in vec2 texcoord;    // ��������
"
"out vec4 fColor;    // ƬԪ������ص���ɫ
"
"uniform sampler2D Texture;  // ����ͼƬ
"
"void main()
"{
"    //fColor.rgb =  texture2D(Texture, texcoord.st).rgb;
"    fColor.rgb =  texture2D(Texture, vec2(texcoord.s, 1.0 - texcoord.t)).rgb;
"    //fColor.rgb = vec3(1, 0, 0);
"}
"