/* +----------------------------------------------------------------------
 * |
 * |                        Lunar Lander Remake 0.1
 * |
 * |  Dirley Rodrigues
 * |  Humberto Rocha
 * |
 * |
 * |
 * +----------------------------------------------------------------------
 */


/* +---------------------------------------------------------------------+
 * |                             Preâmbulo                               |
 * +---------------------------------------------------------------------+
 */
// Includes
#include "bibutil.h"
#include <Box2D/Box2D.h>
#include <vector>

// Variáveis da janela
GLint _x = 800;
GLint _y = 600;

// Variáveis da nave
OBJ *apollo11;
GLfloat _nx;
GLfloat _ny;

GLfloat shipAngle;

//Variáveis do mapa
OBJ *mapa;

// Variáveis do observador
GLfloat angle, fAspect;


// Box2D
b2Vec2 gravity(0.0f, -10.f);
b2World world(gravity);

b2Body *groundBody;
b2Body *shipBody;

float32 worldTimeStep = 1.0f / 60.0f;
int32 worldVelocityIterations = 6;
int32 worldPositionIterations = 2;



/* +---------------------------------------------------------------------+
 * |                        Funções de uso geral                         |
 * +---------------------------------------------------------------------+
 */

// Função usada para especificar o volume de visualização
void AtualizaVisualizacao(void)
{
    // Especifica sistema de coordenadas de projeção
    glMatrixMode(GL_PROJECTION);
    // Inicializa sistema de coordenadas de projeção
    glLoadIdentity();

    // Especifica a projeção perspectiva
    gluPerspective(angle,fAspect,10,500);

    // Especifica sistema de coordenadas do modelo
    glMatrixMode(GL_MODELVIEW);
    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();

    // Especifica posição do observador e do alvo
    gluLookAt(_nx, _ny+10, 60,   _nx, _ny, 0,   0, 1, 0);
}

// Função callback chamada quando o tamanho da janela é alterado 
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    // Para previnir uma divisão por zero
    if ( h == 0 ) h = 1;

    // Especifica o tamanho da viewport
    glViewport(0, 0, w, h);

    // Calcula a correção de aspecto
    fAspect = (GLfloat)w/(GLfloat)h;

    AtualizaVisualizacao();
}

void Teclado (unsigned char tecla, int x, int y)
{
    if (tecla==27) // ESC ?
    {
        // Libera memória e finaliza programa
        LiberaObjeto(apollo11);
        exit(0);
    }

}

void TeclasEspeciais(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP:
        {
            b2Vec2 point;
            b2Vec2 force;

            point.Set(_nx, _ny);
            force.Set(0, 10.0f);

            shipBody->ApplyForce(force, point);
            break; 
        }
        case GLUT_KEY_DOWN:
            _ny-=1;
            break; 
        case GLUT_KEY_LEFT:
            //shipAngle -= 10;
            break; 
        case GLUT_KEY_RIGHT:
            //shipAngle += 10;
            break; 
    }
    AtualizaVisualizacao();
    glutPostRedisplay();
}

// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse(int button, int state, int x, int y)
{

    if (button == GLUT_LEFT_BUTTON)
        if (state == GLUT_DOWN) {  // Zoom-in
            if (angle >= 10)
                angle -= 5;
        }
    if (button == GLUT_RIGHT_BUTTON)
        if (state == GLUT_DOWN) {  // Zoom-out
            if (angle <= 130)
                angle += 5;
        }
    AtualizaVisualizacao();
    glutPostRedisplay();
}

/* +---------------------------------------------------------------------+
 * |                                Física                               |
 * +---------------------------------------------------------------------+
 */

void AtualizarMundo(int value)
{
    world.Step(worldTimeStep,
               worldVelocityIterations,
               worldPositionIterations);
    b2Vec2 position = shipBody->GetPosition();
    float32 angle = shipBody->GetAngle();

    // sincronizar a posição da nave
    _ny = position.y;
    shipAngle = angle;
    glutPostRedisplay();

    // registrar esse mesmo callback novamente
    glutTimerFunc(worldTimeStep * 1000, AtualizarMundo, value+1);
}

void InicializaFisica()
{
    // o chão
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, -10.0f);

    groundBody = world.CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 10.0f);
    groundBody->CreateFixture(&groundBox, 0.0f);


    // a nave
    b2BodyDef shipBodyDef;
    shipBodyDef.type = b2_dynamicBody;
    shipBodyDef.position.Set(0.0f, _ny);
    
    shipBody = world.CreateBody(&shipBodyDef);

    // um triângulo para representar a nave
    b2Vec2 vertices[3];
    vertices[0].Set(0.0, 5.128f);
    vertices[1].Set(-3.345f, 0.0f);
    vertices[2].Set(3.345f, 0.0f);;


    b2PolygonShape shipShape;
    shipShape.Set(vertices, 3);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shipShape;
    fixtureDef.density = 40.0f;
    fixtureDef.friction = 0.3f;

    shipBody->CreateFixture(&fixtureDef);

    // definir um timer para atualizar o mundo
    glutTimerFunc(worldTimeStep * 1000, AtualizarMundo, 0);
}

/* +---------------------------------------------------------------------+
 * |                          Funções de desenho                         |
 * +---------------------------------------------------------------------+
 */

void DesenhaApollo11(void)
{
    DesenhaObjeto(apollo11);

}

void DesenhaMapa(void)
{
    DesenhaObjeto(mapa);

}

/* +---------------------------------------------------------------------+
 * |                            Laço de desenho                          |
 * +---------------------------------------------------------------------+
 */

void Desenha(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DesenhaMapa();

    glTranslatef(_nx, _ny, 0);
    glRotatef(shipAngle, 0, 0, 1);

    DesenhaApollo11();;


    AtualizaVisualizacao();

    glutSwapBuffers();
}

/* +---------------------------------------------------------------------+
 * |                            Inicializações                           |
 * +---------------------------------------------------------------------+
 */

void InicializaLuz(void)
{
    GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0}; 
    GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};	   // "cor" 
    GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};// "brilho" 
    GLfloat posicaoLuz[4]={0.0, 50.0, 50.0, 1.0};

    // Capacidade de brilho do material
    GLfloat especularidade[4]={1.0,1.0,1.0,1.0}; 
    GLint especMaterial = 60;

    // Especifica que a cor de fundo da janela será preta
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Habilita o modelo de colorização de Gouraud
    glShadeModel(GL_FLAT);

    // Define a refletância do material 
    glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);

    // Define a concentração do brilho
    glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

    // Ativa o uso da luz ambiente 
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    // Define os parâmetros da luz de número 0
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente); 
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );

    // Habilita a definição da cor do material a partir da cor corrente
    glEnable(GL_COLOR_MATERIAL);

    //Habilita o uso de iluminação
    glEnable(GL_LIGHTING);  
    // Habilita a luz de número 0
    glEnable(GL_LIGHT0);

    // Habilita o depth-buffering
    glEnable(GL_DEPTH_TEST);

    // !!! Descobrir para que serve !!!
    angle=45;
}

void InicializaLoaders(void) {
    // Carrega o objeto 3D
    apollo11 = CarregaObjeto("nave/apollo11/apollo11.obj", false);

    mapa = CarregaObjeto("mapas/level_1/level_1.obj", false);

    // Seta o modo de desenho
    SetaModoDesenho('s');	// 's' para sólido
}

void Inicializa(void)
{ 
    _nx = 20;
    _ny = 50;
    shipAngle = 0;

    InicializaLuz();

    InicializaLoaders();

    InicializaFisica();
}

/* +---------------------------------------------------------------------+
 * |                                  Main                               |
 * +---------------------------------------------------------------------+
 */

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(_x,_y);
    glutCreateWindow("Lunar Lander Remake");
    glutDisplayFunc(Desenha);
    glutReshapeFunc(AlteraTamanhoJanela);
    glutKeyboardFunc(Teclado);
    glutSpecialFunc(TeclasEspeciais);
    glutMouseFunc(GerenciaMouse);
    Inicializa();
    glutMainLoop();
}

