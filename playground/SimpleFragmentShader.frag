#version 330 core

// Ouput data
out vec4 color;

in vec3 fragmentColor;

in vec3 TexCoord;
uniform sampler2D stoneTexture;
uniform sampler2D waterTexture;
uniform sampler2D snowTexture;
uniform sampler2D grassTexture;
void main()
{
	float normalizedHeight = TexCoord.y/20.0;
	//color = vec4(normalizedHeight,0,0,1);	
	//color = fragmentColor;
	vec4 tex0 = texture(stoneTexture, vec2(TexCoord.x,TexCoord.z)/20.0f);
	vec4 tex1 = texture(waterTexture, vec2(TexCoord.x,TexCoord.z)/20.0f);
	vec4 tex2 = texture(snowTexture, vec2(TexCoord.x,TexCoord.z)/20.0f);
	vec4 tex3 = texture(grassTexture, vec2(TexCoord.x,TexCoord.z)/20.0f);

    //color =mix(tex0,tex2, clamp(normalizedHeight,0,1));

	color = tex0 * vec4(fragmentColor, 1.0);  	
}