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

// Variáveis da janela
GLint _x = 800;
GLint _y = 600;

// Variáveis da nave
OBJ *apollo11;
GLfloat _nx;
GLfloat _ny;

//Variáveis do mapa
OBJ *mapa;

// Variáveis do observador
GLfloat angle, fAspect;



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
            _ny+=1;
            break; 
        case GLUT_KEY_DOWN:
            _ny-=1;
            break; 
        case GLUT_KEY_LEFT:
            _nx-=1;
            break; 
        case GLUT_KEY_RIGHT:
            _nx+=1;
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

    DesenhaApollo11();

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

void Inicializa(void) { 
    _nx = 20;
    _ny = 50;

    InicializaLuz();

    InicializaLoaders();

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

