#version 410

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

const float factor = 0.9f;

uniform vec4 lpos; // light pos in eye space
uniform mat4 Mv;
uniform mat4 Nm;
uniform mat4 Mvp;

in data {
    vec3 pos;
    vec3 normal;
    vec2 texcoord;
} f[];

out data {
    vec3 fragNormal;
    vec3 fragViewDir;
    vec3 fragLightDir;
    vec2 texcoord;
} v;

void main (void)
{
    vec3 cg = vec3(0.0f , 0.0f , 0.0f);
    for (int i = 0; i < gl_in.length(); i++) {
        cg += f[i].pos;
    }
    cg /= float(gl_in.length());
    
    for (int i = 0; i < gl_in.length(); i++) {
        vec3 pos = cg + factor * (f[i].pos - cg);
        vec3 veye = vec3(Mv * vec4(pos, 1.0f));
        
        v.fragViewDir = normalize(-veye);
        if (lpos.w == 0)
            v.fragLightDir = normalize(vec3(lpos));
        else
            v.fragLightDir = normalize(vec3(lpos) - veye);
        v.fragNormal = normalize(vec3(Nm * vec4(f[i].normal, 0.0f)));
        v.texcoord = f[i].texcoord;
        
        gl_Position = Mvp * vec4(pos, 1.0f);
        EmitVertex();
    }
    EndPrimitive();
}
