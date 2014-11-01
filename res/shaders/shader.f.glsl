#version 130

uniform mat4 m_matrix, v_matrix, p_matrix;
uniform sampler2D texture;

varying vec3 frag_position;
varying vec3 frag_normal;
varying vec2 frag_texcoord;
varying float frag_shininess;
varying vec4 frag_specular;

const vec3 light_direction = vec3(0.408248, -0.816497, 0.408248);
const vec4 light_diffuse = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 light_ambient = vec4(0.1, 0.1, 0.1, 0.1);
const vec4 light_specular = vec4(0.8, 0.8, 0.8, 0.8);

void main()
{
    vec3 mv_light_direction = (v_matrix * m_matrix * vec4(light_direction, 0.0)).xyz,
         normal = normalize(frag_normal),
         eye = normalize(frag_position),
         reflection = reflect(mv_light_direction, normal);

    vec4 frag_diffuse = texture2D(texture, frag_texcoord);
    vec4 diffuse_factor
        = max(-dot(normal, mv_light_direction), 0.0) * light_diffuse;
    vec4 ambient_diffuse_factor
        = diffuse_factor + light_ambient;
    vec4 specular_factor
        = max(pow(-dot(reflection, eye), frag_shininess), 0.0) * light_specular;
    
    gl_FragColor = specular_factor * frag_specular
        + ambient_diffuse_factor * frag_diffuse;
    
    vec4 cTexColor = texture2D(texture, frag_texcoord);
    
    if (cTexColor[0] < 0.1 && cTexColor[1] < 0.1 && cTexColor[2] < 0.1) discard;
    
    gl_FragColor = cTexColor;
    
}

