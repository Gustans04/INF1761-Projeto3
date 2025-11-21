#version 410

in vec4 color;
out vec4 fcolor;

in vec3 fragNormal;   // vetor normal (interpolado)
in vec3 fragLightDir; // vetor para luz (interpolado)
in vec3 fragViewDir;  // vetor para observador (interpolado)

uniform vec4 lamb;    // componente ambiente da luz
uniform vec4 ldif;    // componente difusa da luz
uniform vec4 lspe;    // componente especular da luz

uniform vec4 mamb;    // componente ambiente do material
uniform vec4 mdif;    // componente difusa do material
uniform vec4 mspe;    // componente especular do material
uniform float mshi;   // brilho do material
uniform float mopacity; // opacidade do material

void main (void)
{
  vec3 normal = normalize(fragNormal);
  vec3 luz = normalize(fragLightDir);
  vec3 view = normalize(fragViewDir);

  // ambiente e difusa
  vec4 ambiente = mamb * lamb;
  vec4 difusa = max(dot(normal,luz), 0.0) * mdif * ldif;

  // especular
  vec4 espec = vec4(0.0);
  if (dot(normal,luz) > 0) {
    vec3 refl = normalize(reflect(-luz, normal));
    espec = pow(max(dot(refl, view), 0.0), mshi) * mspe * lspe;
  }

  // compõe a cor RGB a partir das componentes e garante que o alfa seja utilizado
  vec3 rgb = (ambiente.rgb + difusa.rgb + espec.rgb);
  float alpha = mopacity; // usar alfa do material (opacidade)
  fcolor = vec4(rgb, alpha);
}

