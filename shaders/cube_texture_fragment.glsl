#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D uTexture;
uniform vec3 uLightPos;
uniform vec3 uViewPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);

    // diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);

    // view direction
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // specular lighting
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = vec3(0.3) * spec;

    vec3 diffuse = vec3(0.8) * diff;
    vec3 ambient = vec3(0.2);

    vec3 lighting = ambient + diffuse + specular;
    vec3 color = texture(uTexture, TexCoord).rgb;

    FragColor = vec4(color * lighting, 1.0);
}