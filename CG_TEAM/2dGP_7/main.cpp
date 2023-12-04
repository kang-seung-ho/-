#include "main.h"
#include "obj.h"

GLuint vao;

GLchar* vertexSource, * fragmentSource;
GLuint vertexShader, fragmentShader;
GLuint shaderProgramID;

GLuint CirclePosVbo;
GLuint CircleNomalVbo;

class obs {
public:
    GLfloat x{}, y{}, z{};
    objRead objReader;
    GLint Object = objReader.loadObj_normalize_center("sphere.obj");
};

obs obstacle;

GLfloat Color[4]{ 0.0f, 0.0f, 0.0f, 1.0f };

void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
void drawScene();
void Reshape(int w, int h);
void InitBuffer();
char* filetobuf(const char*);

void Keyboard(unsigned char key, int x, int y);

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Example1");

    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);

    glewInit();

    make_shaderProgram();
    InitBuffer();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);

    glutKeyboardFunc(Keyboard);

    glutMainLoop();

    return 0;
}


float rotate_light = 0;

float light_x = 0;
float light_y = 0;
float light_z = 0;

float rotate_cube = 0;
float cameraRotation = 0;

float light_r = 1.0f;
float light_g = 1.0f;
float light_b = 1.0f;


void drawScene()
{
    glClearColor(Color[0], Color[1], Color[2], Color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);
    glBindVertexArray(vao);

    int PosLocation = glGetAttribLocation(shaderProgramID, "vPos");
    int NomalLocation = glGetAttribLocation(shaderProgramID, "vNormal");

    glEnableVertexAttribArray(PosLocation);
    glEnableVertexAttribArray(NomalLocation);

    int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
    glUniform3f(lightPosLocation, 0, 0.0, 5.0f);
    int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
    glUniform3f(lightColorLocation, light_r, light_g, light_b);
    int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색

    int modelMatrixLocation = glGetUniformLocation(shaderProgramID, "modelMatrix");
    int viewLoc = glGetUniformLocation(shaderProgramID, "view"); //--- 버텍스 세이더에서 뷰잉 변환 행렬 변수값을 받아온다.
    int projLoc = glGetUniformLocation(shaderProgramID, "projection"); //--- 버텍스 세이더에서 투영 변환 행렬 변수값을 받아온다.

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f); //--- 카메라 위치
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 vTransform(1.0f);
    glm::mat4 pTransform(1.0f);

    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);

    pTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f); //--- 투영 공간 설정: fovy, aspect, near, far

    vTransform = glm::rotate(vTransform, glm::radians(cameraRotation), glm::vec3(0.0f, 1.0f, 0.0f)); // X 축 회전

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);


    int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos"); //--- viewPos 값 전달: 카메라 위치
    glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

    {
        // 모델 행렬 초기화
        glm::mat4 modelMatrix(1.0f);
        // 모델 행렬을 셰이더에 전달

        modelMatrix = glm::translate(modelMatrix, glm::vec3(obstacle.x, obstacle.y, obstacle.z)); // 이동
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniform3f(objColorLocation, 1.0, 0.0, 0.0);

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, CirclePosVbo);
        glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(PosLocation);

        glBindBuffer(GL_ARRAY_BUFFER, CircleNomalVbo);
        glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(NomalLocation);
        glDrawArrays(GL_TRIANGLES, 0, obstacle.Object);

    }

    glDisableVertexAttribArray(PosLocation);
    glDisableVertexAttribArray(NomalLocation);

    glutSwapBuffers();
}


void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void InitBuffer()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &CirclePosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, CirclePosVbo);
    glBufferData(GL_ARRAY_BUFFER, obstacle.objReader.outvertex.size() * sizeof(glm::vec3), &obstacle.objReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &CircleNomalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, CircleNomalVbo);
    glBufferData(GL_ARRAY_BUFFER, obstacle.objReader.outnormal.size() * sizeof(glm::vec3), &obstacle.objReader.outnormal[0], GL_STATIC_DRAW);
}

void make_shaderProgram()
{
    make_vertexShaders();
    make_fragmentShaders();

    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgramID);
}

void make_vertexShaders()
{
    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cout << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}

void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cout << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}

char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb");
    if (!fptr)
        return NULL;
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (char*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;
    return buf;
}

void Keyboard(unsigned char key, int x, int y)
{
    switch (key) {

    case 'z':
        obstacle.z += 1.0f;// 5도씩 자전
        break;
    case 'Z':
        obstacle.z -= 1.0f; // 5도씩 자전
        break;
    }
    InitBuffer();
    glutPostRedisplay();
}