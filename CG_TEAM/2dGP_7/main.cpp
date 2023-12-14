#include "main.h"
#include "obj.h"
#pragma comment(lib, "winmm")
#include <mmsystem.h>

GLuint vao;

GLchar* vertexSource, * fragmentSource;
GLuint vertexShader, fragmentShader;
GLuint shaderProgramID;

GLuint WallPosVbo;
GLuint WallNomalVbo;

GLuint cubePosVbo2;
GLuint cubeNomalVbo2;

GLuint RockPosVbo;
GLuint RockNomalVbo;

GLuint hpPosVbo;
GLuint hpNomalVbo;


GLuint teapotPosVbo;
GLuint teapotNomalVbo;



std::default_random_engine engine(std::random_device{}());

std::uniform_real_distribution<GLfloat> random_scale(0.25f, 0.5f);
std::uniform_real_distribution<GLfloat> random_move(-0.1f, 0.1f);
std::uniform_real_distribution<GLfloat> random_color(0.0f, 1.0f);
std::uniform_real_distribution<double> random_rotate(-10.0f, 10.0f);

std::uniform_real_distribution<GLfloat> random_snow_pos_z(-30.0f, 2.0f);
std::uniform_real_distribution<GLfloat> random_snow_pos_x(-2.0f, 2.0f);
std::uniform_real_distribution<GLfloat> random_snow_pos_y_move(-0.2f, -0.05f);

std::uniform_real_distribution<double> random_model(1, 6);

void setOrthographicProjection() {
    // ���� ��� ��� ����
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // ���� ����Ʈ ��������
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // ���� ���� ��ȯ
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // ���� ���� ��� ����
    gluOrtho2D(0, viewport[2], 0, viewport[3]);

    // �𵨺� ���� �ٽ� ��ȯ
    glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


void renderBitmapString(float x, float y, void* font, const char* string) {
    const char* c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}


class obs {
public:
    GLfloat x{}, y{}, z{-45.0f};

    GLfloat x_scale{2.0f}, y_scale{0.0001f}, z_scale{50.0f};
    objRead objReader;
    GLint Object = objReader.loadObj_normalize_center("cube.obj");

};
class obss {
public:
    GLfloat x{}, y{0.25f}, z{ -1.0f };
    GLfloat x_scale{ 0.25f }, y_scale{ 0.25f }, z_scale{ 0.25f };

    GLfloat r{ 1 }, g{ 0 }, b{ 0 }, a{ 1.0 };
    int jump_scale{};
    int hp = 100;

    GLuint vvbo{cubePosVbo2}, nvbo{cubeNomalVbo2};
    GLint Object{};

    void change_color(float r, float g, float b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    void init(int PosVbo, int NomalVbo) {

        this->vvbo = PosVbo;
        this->nvbo = NomalVbo;
    }

};

class object_won {
public:
    GLfloat x{}, y{ 0.25f }, z{ -100.0f };
    GLfloat x_scale{ 0.25f }, y_scale{ 0.25f }, z_scale{ 0.25f };
    GLfloat x_move{}, y_move{};
    GLfloat r{}, g{}, b{}, a{ 1.0 };
    GLuint vvbo{}, nvbo{};
    GLfloat rotate{};
    GLint object_num{};
    int rotate_move{};


    void init(int PosVbo, int NomalVbo) {
        this->z = -100.0f;
        this->x = 0.0f;
        this->y = 2.0f;
        this->rotate_move = random_rotate(engine);

        this->r = random_color(engine);
        this->g = random_color(engine);
        this->b = random_color(engine);

        float size = random_scale(engine);

        this->x_scale = size;
        this->y_scale = size;
        this->z_scale = size;

        this->x_move = random_move(engine);
        this->y_move = random_move(engine);

        this->vvbo = PosVbo;
        this->nvbo = NomalVbo;
    }

    void move() {
        this->x += this->x_move;
        this->y += this->y_move;

        this->z += 1.0f;

        if (this->z > 5.0f)
        {
            this->init(this->vvbo, this->nvbo);
        }

        if (this->x + this->x_scale + this->x_move > 2.0f || this->x - this->x_scale + this->x_move < -2.0f)
        {
            this->x_move *= -1;
            this->rotate_move *= -1;
        }
        if (this->y + this->y_scale + this->y_move > 4.0f || this->y - this->y_scale + this->y_move < 0.0f)
        {
            this->y_move *= -1;
            this->rotate_move *= -1;
        }

        this->rotate += this->rotate_move;
    }

};

class light_set {
public:
    float rotate_light = 0;

    float light_x = 0;
    float light_y = 8.0f;
    float light_z = 6.0f;

    float rotate_cube = 0;
    float cameraRotation = 0;

    float light_r = 1.0f;
    float light_g = 1.0f;
    float light_b = 1.0f;

    float camera_x = 0;
    float camera_y = 2.0f;
    float camera_z = 5.0f;
};


class bullet {
public:
    GLfloat x{}, y{  }, z{  };
    GLfloat scale{ 0.5f };
    GLfloat z_move{1.0f};
    GLuint vvbo{}, nvbo{};


    void init(float x, float y, float z) {
        this->z = z;
        this->x = x;
        this->y = y;

        this->vvbo = cubePosVbo2;
        this->nvbo = cubeNomalVbo2;
    }

    void move() {
        this->z -= this->z_move;
    }

};

class snow {
public:
    GLfloat x{}, y{  }, z{  };
    GLfloat scale{ 0.025 };
    GLfloat x_move{}, y_move{};
    GLuint vvbo{}, nvbo{};
    


    void init() {
        this->z = random_snow_pos_z(engine);
        this->x = random_snow_pos_x(engine);
        this->y = 4.0f;
        this->y_move = random_snow_pos_y_move(engine);
    }

    void move() {
        this->y += this->y_move;
        if (this->y <= 0)
        {
            this->init();
        }
    }

};

obs wall;
obss main_character;

std::vector<object_won> objects;
std::vector< bullet> bullets;
std::vector<snow> snows;

objRead RockReader;
GLint RockObject = RockReader.loadObj_normalize_center("rock.obj");

objRead CubeReader;
GLint CubeObject = CubeReader.loadObj_normalize_center("cube.obj");

objRead snowReader;
GLint snowObject = snowReader.loadObj_normalize_center("sphere.obj");

objRead teapotReader;
GLint teapotObject = teapotReader.loadObj_normalize_center("teapot.obj");

GLfloat Color[4]{ 0.0f, 0.0f, 0.0f, 1.0f };

bool checkCollision2(object_won& sphere, bullet& wall);
bool checkCollision(object_won& , obss& );


int move_check{};
int jump_check = 3;
int sever_level = 0;
bool game_check = true;
bool left_button = false;
bool shoot_check = true;

int playerHP = 100;

light_set light;
#define GAME_BGM "gamebgm.wav"
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Example1");

    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);
    glutSetCursor(GLUT_CURSOR_NONE);
    glewInit();

    make_shaderProgram();
    InitBuffer();
    glutWarpPointer(800 / 2, 800 / 2);
    glutTimerFunc(1000, next_stage, 1);
    glutTimerFunc(60, update, 1);
    glutTimerFunc(30, shoot_ok, 1);

    main_character.init(cubePosVbo2, cubeNomalVbo2);
    main_character.Object = CubeObject;

    PlaySound(TEXT(GAME_BGM), NULL, SND_ASYNC | SND_LOOP);

    glutKeyboardFunc(keyboard);
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardUpFunc(keyUp);
    glutMouseFunc(MousePoint);
    glutMotionFunc(Motion);

    glutMainLoop();

    return 0;
}

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

    int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
    glUniform3f(lightPosLocation, light.light_x, light.light_y, light.light_z);
    int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
    glUniform3f(lightColorLocation, light.light_r, light.light_g, light.light_b);
    unsigned int lighton = glGetUniformLocation(shaderProgramID, "light");
    glUniform1i(lighton, 1);

    unsigned int ambiont = glGetUniformLocation(shaderProgramID, "amb");

    glUniform3f(ambiont, 0.0,0.0,0.0);

    int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color�� ����: (1.0, 0.5, 0.3)�� ��

    int modelMatrixLocation = glGetUniformLocation(shaderProgramID, "modelMatrix");
    int viewLoc = glGetUniformLocation(shaderProgramID, "view"); //--- ���ؽ� ���̴����� ���� ��ȯ ��� �������� �޾ƿ´�.
    int projLoc = glGetUniformLocation(shaderProgramID, "projection"); //--- ���ؽ� ���̴����� ���� ��ȯ ��� �������� �޾ƿ´�.

    glm::vec3 cameraPos = glm::vec3(light.camera_x, light.camera_y, light.camera_z); //--- ī�޶� ��ġ
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -200.0f); //--- ī�޶� �ٶ󺸴� ����
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 vTransform(1.0f);
    glm::mat4 pTransform(1.0f);

    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);

    vTransform = glm::rotate(vTransform, glm::radians(light.cameraRotation), glm::vec3(0.0f, 0.0f, 1.0f)); // z������ ȸ��

    pTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 200.0f); //--- ���� ���� ����: fovy, aspect, near, far

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);

    int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos"); //--- viewPos �� ����: ī�޶� ��ġ
    glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

    //�����¿� �� �׸��� �ݺ���
    for (int i = 0; i < 4; i++) {
        // �� ��� �ʱ�ȭ
        glm::mat4 modelMatrix(1.0f);
        // �� ����� ���̴��� ����
        if (light.cameraRotation == 180) {
            if (i == 0) {

                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 4.0f, 0)); // �̵�
            }
            else if (i == 1)
            {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.0f, 2.0f, 0)); // �̵�
                modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X �� ȸ��
            }
            else if (i == 2)
            {
                modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X �� ȸ��
            }
            else if (i == 3)
            {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 2.0f, 0.0f)); // �̵�
                modelMatrix = glm::rotate(modelMatrix, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X �� ȸ��
            }
        }
        else
        {
            if (i == 0) {

            }
            else if (i == 1)
            {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 2.0f, 0.0f)); // �̵�
                modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X �� ȸ��
            }
            else if (i == 2)
            {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 4.0f, 0)); // �̵�
                modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X �� ȸ��
            }
            else if (i == 3)
            {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.0f, 2.0f, 0)); // �̵�
                modelMatrix = glm::rotate(modelMatrix, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X �� ȸ��
            }
        }
        modelMatrix = glm::translate(modelMatrix, glm::vec3(wall.x, wall.y, wall.z)); // �̵�
        modelMatrix = glm::scale(modelMatrix, glm::vec3(wall.x_scale, wall.y_scale, wall.z_scale));
        glUniform4f(objColorLocation, 0.0, 0.0, 1.0,1.0);

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, WallPosVbo);
        glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(PosLocation);

        glBindBuffer(GL_ARRAY_BUFFER, WallNomalVbo);
        glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(NomalLocation);

        glDrawArrays(GL_TRIANGLES, 0, wall.Object);

    }

    //���� ĳ���� �׸��� �ݺ���
    {
        // �� ��� �ʱ�ȭ
        glm::mat4 modelMatrix(1.0f);
        // �� ����� ���̴��� ����
        modelMatrix = glm::translate(modelMatrix, glm::vec3(main_character.x, main_character.y, main_character.z)); // �̵�
        modelMatrix = glm::scale(modelMatrix, glm::vec3(main_character.x_scale, main_character.y_scale, main_character.z_scale));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(light.cameraRotation), glm::vec3(0.0f, 0.0f, 1.0f)); // z������ ȸ��
        glUniform4f(objColorLocation, main_character.r, main_character.g, main_character.b, 1.0);

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, main_character.vvbo);
        glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(PosLocation);

        glBindBuffer(GL_ARRAY_BUFFER, main_character.nvbo);
        glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(NomalLocation);

        glDrawArrays(GL_TRIANGLES, 0, main_character.Object);

    }
    //��ֹ��� �׸��� �ݺ���
    for (int i = 0; i < objects.size(); i++){
        // �� ��� �ʱ�ȭ
        glm::mat4 modelMatrix(1.0f);
        // �� ����� ���̴��� ����
        modelMatrix = glm::translate(modelMatrix, glm::vec3(objects[i].x, objects[i].y, objects[i].z)); // �̵�

        modelMatrix = glm::rotate(modelMatrix, glm::radians(objects[i].rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // X �� ȸ��
        modelMatrix = glm::rotate(modelMatrix, glm::radians(objects[i].rotate), glm::vec3(1.0f, 0.0f, 0.0f)); // X �� ȸ��
        modelMatrix = glm::rotate(modelMatrix, glm::radians(objects[i].rotate), glm::vec3(0.0f, 1.0f, 0.0f)); // X �� ȸ��
        modelMatrix = glm::scale(modelMatrix, glm::vec3(objects[i].x_scale, objects[i].y_scale, objects[i].z_scale));
        glUniform4f(objColorLocation, objects[i].r, objects[i].g, objects[i].b, objects[i].a);

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, objects[i].vvbo);
        glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(PosLocation);

        glBindBuffer(GL_ARRAY_BUFFER, objects[i].nvbo);
        glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(NomalLocation);

        if (sever_level > 2) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        glDrawArrays(GL_TRIANGLES, 0, objects[i].object_num);

        if (sever_level >2 ) {
            glDisable(GL_BLEND);
        }
    }

    for (int i = 0; i < bullets.size(); i++) {
        // �� ��� �ʱ�ȭ
        glm::mat4 modelMatrix(1.0f);
        // �� ����� ���̴��� ����
        modelMatrix = glm::translate(modelMatrix, glm::vec3(bullets[i].x, bullets[i].y, bullets[i].z)); // �̵�
        modelMatrix = glm::scale(modelMatrix, glm::vec3(bullets[i].scale, bullets[i].scale, bullets[i].scale));
        glUniform4f(objColorLocation, 1.0f,1.0f,1.0f,1.0f);

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, bullets[i].vvbo);
        glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(PosLocation);

        glBindBuffer(GL_ARRAY_BUFFER, bullets[i].nvbo);
        glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(NomalLocation);

        if (sever_level > 2) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        glDrawArrays(GL_TRIANGLES, 0, CubeObject);

        if (sever_level > 2) {
            glDisable(GL_BLEND);
        }
    }

    if (sever_level >= 4) {
        for (int i = 0; i < snows.size(); i++) {
            // �� ��� �ʱ�ȭ
            glm::mat4 modelMatrix(1.0f);
            // �� ����� ���̴��� ����
            modelMatrix = glm::translate(modelMatrix, glm::vec3(snows[i].x, snows[i].y, snows[i].z)); // �̵�
            modelMatrix = glm::scale(modelMatrix, glm::vec3(snows[i].scale, snows[i].scale, snows[i].scale));

            glUniform4f(objColorLocation, 1.0f, 1.0f, 1.0f, 1.0);

            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

            glBindBuffer(GL_ARRAY_BUFFER, hpPosVbo);
            glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
            glEnableVertexAttribArray(PosLocation);

            glBindBuffer(GL_ARRAY_BUFFER, hpNomalVbo);
            glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
            glEnableVertexAttribArray(NomalLocation);

            glDrawArrays(GL_TRIANGLES, 0, snowObject);

        }
    }
    glDisableVertexAttribArray(PosLocation);
    glDisableVertexAttribArray(NomalLocation);

    // ���� ����Ʈ ũ�⸦ ����ϴ�.
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int windowWidth = viewport[2];
    int windowHeight = viewport[3];

    // ���� ���� ����
    setOrthographicProjection();

    glUseProgram(0);

    // �ؽ�Ʈ ������
    glPushMatrix();
    glLoadIdentity();

    // �ؽ�Ʈ�� ��ġ�� ȭ�� ���� ������� ����
    float x = windowWidth - 100; // ȭ�� �ʺ񿡼� 100px ������ ��ġ
    float y = windowHeight - 30; // ȭ�� ���̿��� 30px ������ ��ġ
    renderBitmapString(x, y, GLUT_BITMAP_HELVETICA_18, ("HP: " + std::to_string(main_character.hp)).c_str());

    glPopMatrix();

    // ���� �������� ����
    resetPerspectiveProjection();

    glUseProgram(shaderProgramID);


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

    glGenBuffers(1, &WallPosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, WallPosVbo);
    glBufferData(GL_ARRAY_BUFFER, wall.objReader.outvertex.size() * sizeof(glm::vec3), &wall.objReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &WallNomalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, WallNomalVbo);
    glBufferData(GL_ARRAY_BUFFER, wall.objReader.outnormal.size() * sizeof(glm::vec3), &wall.objReader.outnormal[0], GL_STATIC_DRAW);

    glGenBuffers(1, &cubePosVbo2);
    glBindBuffer(GL_ARRAY_BUFFER, cubePosVbo2);
    glBufferData(GL_ARRAY_BUFFER,CubeReader.outvertex.size() * sizeof(glm::vec3), &CubeReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &cubeNomalVbo2);
    glBindBuffer(GL_ARRAY_BUFFER, cubeNomalVbo2);
    glBufferData(GL_ARRAY_BUFFER, CubeReader.outnormal.size() * sizeof(glm::vec3), &CubeReader.outnormal[0], GL_STATIC_DRAW);

    glGenBuffers(1, &RockPosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, RockPosVbo);
    glBufferData(GL_ARRAY_BUFFER, RockReader.outvertex.size() * sizeof(glm::vec3), &RockReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &RockNomalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, RockNomalVbo);
    glBufferData(GL_ARRAY_BUFFER, RockReader.outnormal.size() * sizeof(glm::vec3), &RockReader.outnormal[0], GL_STATIC_DRAW);


    glGenBuffers(1, &hpPosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, hpPosVbo);
    glBufferData(GL_ARRAY_BUFFER, snowReader.outvertex.size() * sizeof(glm::vec3), &snowReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &hpNomalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, hpNomalVbo);
    glBufferData(GL_ARRAY_BUFFER, snowReader.outnormal.size() * sizeof(glm::vec3), &snowReader.outnormal[0], GL_STATIC_DRAW);

    glGenBuffers(1, &teapotPosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, teapotPosVbo);
    glBufferData(GL_ARRAY_BUFFER, teapotReader.outvertex.size() * sizeof(glm::vec3), &teapotReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &teapotNomalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, teapotNomalVbo);
    glBufferData(GL_ARRAY_BUFFER, teapotReader.outnormal.size() * sizeof(glm::vec3), &teapotReader.outnormal[0], GL_STATIC_DRAW);

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
        std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
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
        std::cout << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
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



GLvoid update(int value) {

    for (int i = 0; i < objects.size(); i++)
    {
        objects[i].move();

        if (checkCollision(objects[i], main_character)) {
            // �浹 �߻� �� ���� ����ϴ�.
            objects[i].z = -200.0f; // ���� ��ġ�� ȭ�� ������ �̵�
            main_character.hp -= 10;
            main_character.init(objects[i].vvbo, objects[i].nvbo);
            main_character.Object = objects[i].object_num;

            if (main_character.hp <= 0)
            {
                objects.clear();
                snows.clear();
                bullets.clear();
                sever_level = 0;
                game_check = false;
            }
            else
            {
                main_character.change_color(objects[i].r, objects[i].g, objects[i].b);
            }
        }
    }

    for (int i = bullets.size() - 1; i >= 0; --i) {

        bullets[i].move();
        if (objects.size() > 0) {
            for (int k = 0; k < objects.size(); k++) {

                if (bullets[i].z < -200 || checkCollision2(objects[k], bullets[i])) {
                    std::swap(bullets[i], bullets.back());
                    bullets.pop_back();

                    if (sever_level > 1) {
                        objects[k].init(RockPosVbo, RockNomalVbo); // ��ü �ʱ�ȭ
                    }
                    else {
                        objects[k].init(cubePosVbo2, cubeNomalVbo2); // ��ü �ʱ�ȭ
                    }
                    break;
                }
            }
        }
        else {
            if (bullets[i].z < -100) {
                std::swap(bullets[i], bullets.back());
                bullets.pop_back();
            }
        }
    }

    if (sever_level >3 )
    {
        for (int i = 0; i < snows.size(); i++)
        {
            snows[i].move();

        }
    }

    if (light.cameraRotation == 0)
    {
        main_character.y = 0.25f + 0.1f * main_character.jump_scale;
        light.light_y = 8.0f;

    }
    else if (light.cameraRotation == 270)
    {
        main_character.x = 2.0f - main_character.x_scale - 0.1f * main_character.jump_scale;
        light.light_y = 8.0f;

    }
    else if (light.cameraRotation == 180)
    {
        main_character.y = 4.0f - main_character.y_scale - 0.1f * main_character.jump_scale;
        light.light_y = -4.0f;
    }
    else if (light.cameraRotation == 90)
    {
        main_character.x = -2.0f + main_character.x_scale + 0.1f * main_character.jump_scale;
        light.light_y = 8.0f;
    }

    InitBuffer();
    glutPostRedisplay();

    if (game_check) {
        glutTimerFunc(30, update, 1);
    }
}

GLvoid keyboard(unsigned char key, int x, int y)
{
    handleEvent(key, true);
    glutPostRedisplay();
}
GLvoid keyUp(unsigned char key, int x, int y)
{
    handleEvent(key, false);
    glutPostRedisplay();
}

GLvoid handleEvent(unsigned char key, bool state)
{
    if (state)
    {
        switch (key) {
        case 'z':
            jump();
            break;
        case 'c':
            if (shoot_check)
            {
                shoot_check = false;
                shoot();
            }
            break;
        case '2':
            if (sever_level != 2) {
                sever_level = 2;
                glutTimerFunc(90, object_ok, 1);
            }
            break;
        case '3':
            if (sever_level != 3) {
                sever_level = 3;
                objects.clear();
                glutTimerFunc(90, object_ok, 1);
            }
            break;
        case '4':
            if (sever_level != 4) {
                sever_level = 4;

                snow_init(1);
            }
            break;
        case 'r':
            game_check = true;
            sever_level = 0;
            objects.clear();

            break;

        }
    }
}

GLvoid MousePoint(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_UP) {
           left_button = false;
            glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

        }
        else {
           left_button = true;
            glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
        }
    }
}

GLvoid Motion(int x, int y) {
    {
        if (left_button) {
            glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
            GLfloat mouseX2 = static_cast<GLfloat>(x) - (glutGet(GLUT_WINDOW_WIDTH) / 2.0f);

            // ������ �߽�(400)���� ���콺 ��ġ�� �Ÿ� ���
            GLfloat distanceFromCenter = mouseX2; // ������ �߽��� �������� �Ÿ� ���
            GLfloat rotationChange = distanceFromCenter * 0.005f; // �ʿ信 ���� ���� ����

            
            if (light.cameraRotation == 0)
            {
                main_character.y = 0.25f + 0.1f * main_character.jump_scale;
                main_character.x += rotationChange;
                light.light_y = 8.0f;
            }
            else if (light.cameraRotation == 270)
            {
                main_character.x = 2.0f - main_character.x_scale - 0.1f * main_character.jump_scale;
                main_character.y += rotationChange;
                light.light_y = 8.0f;

            }
            else if (light.cameraRotation == 180)
            {
                main_character.y = 4.0f - main_character.y_scale - 0.1f * main_character.jump_scale;
                main_character.x -= rotationChange;
                light.light_y = -4.0f;
            }
            else if (light.cameraRotation == 90)
            {
                main_character.x = -2.0f + main_character.x_scale + 0.1f * main_character.jump_scale;
                main_character.y -= rotationChange;
                light.light_y = 8.0f;
            }


            if(sever_level > 3)
            {
                if (main_character.x + main_character.x_scale > 2.0f || main_character.x - main_character.x_scale < -2.0f)
                {
                    main_character.x -= rotationChange;
                }
            }
            // ���콺�� ������ �߾����� �̵�
            glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
        }
        if(sever_level < 4){
            if (main_character.x + main_character.x_scale > 2.0f)
            {
                light.cameraRotation = 270.0f;
                light.camera_x = 2.0f;
                light.camera_y = 0;
                jump_check = 3;
                main_character.jump_scale = 0;
            }
            else if (main_character.x - main_character.x_scale < -2.0f)
            {
                light.cameraRotation = 90.0f;
                light.camera_x = -2.0f;
                light.camera_y = 0.0f;
                jump_check = 3;
                main_character.jump_scale = 0;
            }
            else if (main_character.y - main_character.y_scale < 0.0f)
            {
                light.cameraRotation = 0.0f;
                light.camera_x = 0.0f;
                light.camera_y = 2.0f;
                jump_check = 3;
                main_character.jump_scale = 0;
            }
            else if (main_character.y + main_character.y_scale > 4.0f)
            {
                light.cameraRotation = 180.0f;
                light.camera_x = 0.0f;
                light.camera_y = -2.0f;
                main_character.jump_scale = 0;
                jump_check = 3;
            }
        }
        
        InitBuffer();
        glutPostRedisplay();
    }
}

GLvoid jump() {
    if (light.cameraRotation == 0)
    {
        if (jump_check == 3) {
            jump_check = 0;
            main_character.jump_scale = 0;
            glutTimerFunc(60, jump_ok, 1);
        }
    }
    else if (light.cameraRotation == 270)
    {
        if (jump_check == 3) {
            jump_check = 0;
            main_character.jump_scale = 0;
            glutTimerFunc(60, jump_ok, 1);
        }
    }
    else if (light.cameraRotation == 180)
    {
        if (jump_check == 3) {
            jump_check = 0;
            main_character.jump_scale = 0;
            glutTimerFunc(60, jump_ok, 1);
        }
    }
    else if (light.cameraRotation == 90)
    {

        if (jump_check == 3) {
            jump_check = 0;
            main_character.jump_scale = 0;
            glutTimerFunc(60, jump_ok, 1);
        }
    }
}

bool checkCollision(object_won& sphere, obss& wall) {
    // AABB - �� �浹
    float closestX = std::max(wall.x - wall.x_scale, std::min(sphere.x, wall.x + wall.x_scale));
    float closestY = std::max(wall.y - wall.y_scale, std::min(sphere.y, wall.y + wall.y_scale));
    float closestZ = std::max(wall.z - wall.z_scale, std::min(sphere.z, wall.z + wall.z_scale));

    // ���� �߽ɰ� ���� ����� �� ������ �Ÿ��� ���
    float distanceX = sphere.x - closestX;
    float distanceY = sphere.y - closestY;
    float distanceZ = sphere.z - closestZ;

    // �Ÿ��� ���� ���������� ������ ����������
    float radius = sphere.x_scale;
    return (distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ) < (radius * radius);
}

bool checkCollision2(object_won& sphere, bullet& wall) {
    // AABB - �� �浹
    float closestX = std::max(wall.x - wall.scale, std::min(sphere.x, wall.x + wall.scale));
    float closestY = std::max(wall.y - wall.scale, std::min(sphere.y, wall.y + wall.scale));
    float closestZ = std::max(wall.z - wall.scale, std::min(sphere.z, wall.z + wall.scale));

    // ���� �߽ɰ� ���� ����� �� ������ �Ÿ��� ���
    float distanceX = sphere.x - closestX;
    float distanceY = sphere.y - closestY;
    float distanceZ = sphere.z - closestZ;

    // �Ÿ��� ���� ���������� ������ ����������
    float radius = sphere.x_scale;
    return (distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ) < (radius * radius);
}

GLvoid jump_ok(int value) {

    if (jump_check != 3) {
    if (main_character.jump_scale < 15 && jump_check == 0) {
        main_character.jump_scale += 1;
        if (main_character.jump_scale == 15)
        {
            jump_check = 1;
        }
    }
    else if (main_character.jump_scale > 0 && jump_check == 1) {
        main_character.jump_scale -= 1;
        if (main_character.jump_scale == 0)
        {
            jump_check = 3;
        }
    }
    InitBuffer();
    glutPostRedisplay();
    glutTimerFunc(30, jump_ok, 1);
    }
}

GLvoid object_ok(int value) {

    if (objects.size() < 10) {
        object_won new_object;
        if (sever_level > 1) {
            int model = random_model(engine);
            if (model == 1) {
                new_object.init(RockPosVbo, RockNomalVbo); // ��ü �ʱ�ȭ
                new_object.object_num = RockObject;
            }
            else if (model == 2)
            {
                new_object.init(cubePosVbo2, cubeNomalVbo2); // ��ü �ʱ�ȭ
                new_object.object_num = CubeObject;
            }
            else if( model == 3)
            {
                new_object.init(hpPosVbo, hpNomalVbo); // ��ü �ʱ�ȭ
                new_object.object_num = snowObject;
            }
            else
            {
                new_object.init(teapotPosVbo, teapotNomalVbo); // ��ü �ʱ�ȭ
                new_object.object_num = teapotObject;
            }
        }
        else {
            new_object.init(cubePosVbo2, cubeNomalVbo2); // ��ü �ʱ�ȭ
            new_object.object_num = CubeObject;
        }
        if (sever_level > 2) {
            new_object.a = 0.1f;
        }
        else
        {
            new_object.a = 1.0f;
        }
        objects.push_back(new_object);

        InitBuffer();
        glutPostRedisplay();

        if (sever_level >0) {
            glutTimerFunc(480, object_ok, 1);
        }
    }
}

GLvoid snow_init(int value) {

    for(int i = 0; i<200;i++){
        snow new_object;
        new_object.init(); // ��ü �ʱ�ȭ
        snows.push_back(new_object);
    }
    InitBuffer();
    glutPostRedisplay();
}

GLvoid next_stage(int value) {
    if (game_check) {


        std::cout << sever_level << std::endl;
        if (sever_level < 5) {
            sever_level++;
        }

        if (sever_level == 1) {

            objects.clear();
            glutTimerFunc(900, object_ok, 1);
        }
        else if (sever_level == 2) {

            objects.clear();
            glutTimerFunc(900, object_ok, 1);
        }
        else if (sever_level == 3) {
            objects.clear();
            glutTimerFunc(900, object_ok, 1);
        }
        else if (sever_level == 4) {
            light.cameraRotation = 0.0f;
            light.camera_x = 0.0f;
            light.camera_y = 2.0f;
            jump_check = 3;
            main_character.jump_scale = 0;
            main_character.x = 0;
            main_character.y = 0.25f;
            main_character.z = -1.0f;
            objects.clear();
            glutTimerFunc(900, object_ok, 1);
            snow_init(1);
        }

        glutTimerFunc(10000, next_stage, 1);
        InitBuffer();
        glutPostRedisplay();
    }
}

GLvoid shoot() {
    bullet new_object;
    new_object.init(main_character.x, main_character.y, main_character.z); // ��ü �ʱ�ȭ
    bullets.push_back(new_object);
}

GLvoid shoot_ok(int value) {
    if (!shoot_check)
    {
        shoot_check = true;
    }

    InitBuffer();
    glutPostRedisplay();

    glutTimerFunc(3000, shoot_ok, 1);

}