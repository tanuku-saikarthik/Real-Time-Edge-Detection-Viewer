attribute vec4 aPosition;
attribute vec2 aTexCoord;
varying vec2 vTexCoord;
void main() {
    gl_Position = aPosition;
    vTexCoord = aTexCoord;
}

precision mediump float;
varying vec2 vTexCoord;
uniform sampler2D uTexture;
void main() {
    float l = texture2D(uTexture, vTexCoord).r;
    gl_FragColor = vec4(l, l, l, 1.0);
}
