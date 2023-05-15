#version 430 core

uniform vec3 CameraPos;
uniform bool DrawLine;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} fs_in;

out vec4 FragColor;
vec3 CalcDirLight(
    vec3 normal,
    vec3 viewDirection,
    vec3 lightDirection,
    vec3 intensity,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular,
    int shiniess
);

void main()
{
    if (DrawLine) {
        FragColor = vec4(0.5, 0.5, 0.5, 1);
    } else {
        vec3 localColor = vec3(0);
        vec3 viewDirection = normalize(fs_in.FragPos - CameraPos);
        localColor = (
            CalcDirLight(
                fs_in.Normal,
                viewDirection,
                normalize(vec3(0, -0.6, -1)),
                vec3(1.0, 1.0, 1.0),
                vec3(0.2, 0.2, 0.2),
                vec3(0.5, 0.5, 0.5),
                vec3(0.3, 0.3, 0.3),
                2
            )
        );
        FragColor = vec4(localColor, 1);
    }
}


vec3 CalcDirLight(
    vec3 normal,
    vec3 viewDirection,
    vec3 lightDirection,
    vec3 intensity,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular,
    int shiniess
) {
    float dpLightNormal = dot(-lightDirection, normal);
    diffuse = diffuse * max(dpLightNormal, 0.0f);

    const bool useBlinn = false;
    if (dpLightNormal > 0) {
        if (useBlinn) {
            vec3 halfwayDirection = -normalize(viewDirection + lightDirection);
            float dpHalfwayNormal = dot(halfwayDirection, normal);

            specular = specular * pow(max(dpHalfwayNormal, 0), int(shiniess));
        } else {
            vec3 reflectDirection = reflect(lightDirection, normal);

            float dpReflectView = dot(reflectDirection, viewDirection);
            specular = specular * pow(max(dpReflectView, 0), int(shiniess));
        }
    }
    return (ambient + diffuse + specular) * intensity;
}