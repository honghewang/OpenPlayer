precision highp float;

varying   highp vec2 vv2_texcoord;

attribute highp vec4 av4_position;
attribute highp vec2 av2_texcoord;

void main() {
    vv2_texcoord = av2_texcoord;
    gl_Position  = av4_position;
}
