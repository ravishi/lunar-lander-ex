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
#include <stdio.h>
#include <Box2D/Box2D.h>
#include <math.h>
#include <map>


/* +---------------------------------------------------------------------+
 * |                         Definições e Tipos                          |
 * +---------------------------------------------------------------------+
 */

#define RAD_TO_GRAUS (180 / M_PI)

#define IMPACTO_TOLERAVEL 50

#define ANGULO_TOLERAVEL 10


/**
 * Um simples tanque de combustível.
 */
class TanqueCombustivel {
    public:
        TanqueCombustivel(GLfloat combustivel)
        {
            _combustivel = combustivel;
        }

        GLfloat combustivel() const {
            return _combustivel;
        }

        /**
         * Gasta uma quantidade de combustível do tanque. Retorna um valor
         * menor ou igual à quantidade solicitada, dependendo da quantidade de
         * combustível presente no tanque.
         */
        GLfloat gastar(GLfloat quant) {
            if (_combustivel <= 0)
                return 0;

            if (quant >= _combustivel) {
                quant = _combustivel;
                _combustivel = 0;
            } else {
                _combustivel -= quant;
            }

            return quant;
        }

    private:
        GLfloat _combustivel;
};


/**
 * Um simples motor. Motores possuem uma potência máxima, um tempo de
 * inicialização (o tempo que o motor leva para chegar à potência máxima) e
 * podem ter um tanque de combustível associado.
 *
 * Enquanto o motor estiver ligado, sua potência será incrementada
 * gradativamente, até atingir a potência máxima. Quando o motor for
 * desligador, sua potência decrescerá gradativamente até zero. Se o
 * combustível acabar, o motor parará de funcionar (sua potência será sempre
 * zero).
 *
 * Quando houver um tanque associado, o motor gastará combustível do tanque e
 * sua potência será cortada quando o combustível no tanque acabar.
 */
class Motor {
    public:
        /**
         * potencia: a potência do motor.
         * tempo_inic: o tempo de inicialização do motor. dado em milisegundos.
         * tanque: um tanque de combustível. pode ser NULL, indicando que o
         *         motor não gastará combustível.
         */
        Motor(GLfloat potencia, unsigned int tempo_inic = 1000, TanqueCombustivel *tanque = NULL)
        {
            _ligado = false;
            _tanque = tanque;
            _potencia = 0;
            _potencia_max = potencia;
            _tempo_inic = tempo_inic;
        }

        void setTanque(TanqueCombustivel *tanque)
        {
            _tanque = tanque;
        }

        bool ligado() const {
            return _ligado;
        }

        /**
         * Desliga o motor.
         */
        bool desligar() {
            if (!ligado()) {
                return false;
            }
            _ligado = false;
        }

        /**
         * Liga o motor.
         */
        bool ligar() {
            if (ligado()) {
                return false;
            }
            _ligado = true;
            _potencia = 0;
        }

        GLfloat potencia() const {
            return _potencia;
        }

        /**
         * Atualiza o motor. Step é o tempo desde a última atualização (dado em
         * milisegundos).
         */
        void atualizar(GLfloat step) {
            if (step <= 0)
                return;

            if (_tanque != NULL && !_tanque->combustivel())
            {
                _potencia = 0;
                return;
            }

            GLfloat old = _potencia;

            if (ligado() && _potencia < _potencia_max) {
                _potencia += (_potencia_max / _tempo_inic) * step;
                if (_potencia > _potencia_max) {
                    _potencia = _potencia_max;
                }
            } else if (!ligado() && _potencia > 0) {
                _potencia -= (_potencia_max / _tempo_inic) * step;
                if (_potencia < 0) {
                    _potencia = 0;
                }
            }

            GLfloat media = (_potencia + old) / 2;
            if (_tanque != NULL && media) {
                // TODO calcular um multiplicador mais adequado para o fator
                GLfloat fator = step * 0.001;
                _potencia = _tanque->gastar(media * fator) / fator;
            }
        }

    private:
        bool _ligado;
        GLfloat _potencia;
        GLfloat _potencia_max;
        unsigned int _tempo_inic;
        TanqueCombustivel *_tanque;
};

// Listener para a colisão
class ContactListener: public b2ContactListener
{
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
    {
        bool explode = false;
        GLfloat impacto = impulse->normalImpulses[0];

        // verificar o impacto da colisão
        if (impacto > IMPACTO_TOLERAVEL)
        {
            explode = true;

            printf("Impacto: %.4f\n", impacto);
        }

        // verificar o ângulo de colisão
        if (!explode) {
            // XXX encontrar a nave. a nave é o único corpo dinâmico
            // em nosso mundo, então é so testar isso ;D
            b2Body *nave;
            if (contact->GetFixtureA()->GetBody()->GetType() == b2_dynamicBody)
                nave = contact->GetFixtureA()->GetBody();
            else
                nave = contact->GetFixtureB()->GetBody();

            // o ângulo da nave
            GLfloat angulo = nave->GetAngle();

            // a nave explode se o ângulo de impacto estiver
            // fora dos limites
            if (angulo < -ANGULO_TOLERAVEL || angulo > ANGULO_TOLERAVEL) {
                explode = true;

                printf("Você caiu com um ângulo de %.4f graus.\n", angulo);
            }
        }

        if (explode) {
            printf("Você explodiu!\n");
        }
    }
};


/* +---------------------------------------------------------------------+
 * |                          Variáveis Globais                          |
 * +---------------------------------------------------------------------+
 */

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
TEX *texFundo;

// Variáveis do observador
// Angulo de abertura da câmera e aspecto de visão
GLfloat camOpeningAngle, fAspect;


// Box2D
b2Vec2 gravity(0.0f, -10.0f);
b2World world(gravity);

// os corpos
b2Body *groundBody;
b2Body *shipBody;

// constantes para a simulação da física
float32 worldTimeStep = 1.0f / 60.0f;
int32 worldVelocityIterations = 6;
int32 worldPositionIterations = 2;

// motores
Motor motorPrincipal(500);
Motor motorLatEsq(50);
Motor motorLatDir(50);

TanqueCombustivel *tanque = NULL;

// listener para detectar o momento da colisão
ContactListener listenerDeContato;


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
    gluPerspective(camOpeningAngle,fAspect,10, 500);

    // Especifica sistema de coordenadas do modelo
    glMatrixMode(GL_MODELVIEW);
    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();

    // Especifica posição do observador e do alvo
    gluLookAt(_nx, 30, 80,   _nx, _ny, 0,   0, 1, 0);
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

void Teclado(unsigned char tecla, int x, int y)
{
    if (tecla==27) // ESC ?
    {
        // Libera memória e finaliza programa
        LiberaObjeto(apollo11);
        LiberaMateriais();
        exit(0);
    }

}

void TeclasEspeciais(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP:
            motorPrincipal.ligar();
            break;
        case GLUT_KEY_LEFT:
            motorLatEsq.ligar();
            break;
        case GLUT_KEY_RIGHT:
            motorLatDir.ligar();
            break;
            /*
        case GLUT_KEY_DOWN:
        {
            if (pressedKeys.count(key) <= 0) {
                pressedKeys[key] = 0;
            }
            break; 
        }
        */
    }
}

void SpecialKeyUp(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP:
            motorPrincipal.desligar();
            break;
        case GLUT_KEY_LEFT:
            motorLatEsq.desligar();
            break;
        case GLUT_KEY_RIGHT:
            motorLatDir.desligar();
            break;
            /*
        case GLUT_KEY_DOWN:
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
        {
            pressedKeys.erase(key);
            break; 
        }
        */
    }
}

// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse(int button, int state, int x, int y)
{

    if (button == GLUT_LEFT_BUTTON)
        if (state == GLUT_DOWN) {  // Zoom-in
            if (camOpeningAngle >= 10)
                camOpeningAngle -= 5;
        }
    if (button == GLUT_RIGHT_BUTTON)
        if (state == GLUT_DOWN) {  // Zoom-out
            if (camOpeningAngle <= 130)
                camOpeningAngle += 5;
        }
    AtualizaVisualizacao();
    glutPostRedisplay();
}

void Escreva(char *string){//Write string on the screen
    while(*string)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *string++);
}

/* +---------------------------------------------------------------------+
 * |                                Física                               |
 * +---------------------------------------------------------------------+
 */

void AtualizarMundo(int value)
{
    // o step do mundo. vamos precisar desse valor para chamar alguns métodos,
    // então é melhor colocá-lo em uma variável local.
    GLfloat worldStep = worldTimeStep * 1000;

    // o ângulo atual da nave (radianos)
    GLfloat angulo = shipBody->GetAngle();

    // atualizar o motor principal
    motorPrincipal.atualizar(worldStep);

    // aplicar sobre a nave a força gerada pelo motor principal
    b2Vec2 force(
            motorPrincipal.potencia() * cos(M_PI / 2 + angulo),
            motorPrincipal.potencia() * sin(M_PI / 2 + angulo)
        );

    // a força é aplicada sobre o ponto (0, 0) [a base] da nave
    shipBody->ApplyForce(force, shipBody->GetWorldPoint(b2Vec2(0, 0)));

    // atualizar motores laterais, aplicar suas forças sobre a nave, etc
    motorLatDir.atualizar(worldStep);
    force.Set(
            motorLatDir.potencia() * cos(M_PI + angulo),
            motorLatDir.potencia() * sin(M_PI + angulo)
        );
    shipBody->ApplyForce(force, shipBody->GetWorldPoint(b2Vec2(0, 5)));

    motorLatEsq.atualizar(worldStep);
    force.Set(
            motorLatEsq.potencia() * cos(angulo),
            motorLatEsq.potencia() * sin(angulo)
        );
    shipBody->ApplyForce(force, shipBody->GetWorldPoint(b2Vec2(0, 5)));

    // o gasto de combustível deve associar, de alguma forma, a potência atual
    // do motor ao tempo que o motor fica ligado.
    GLfloat potencia = motorLatEsq.potencia() + motorLatDir.potencia() + motorPrincipal.potencia();

    // simular a física do mundo
    world.Step(worldTimeStep,
               worldVelocityIterations,
               worldPositionIterations);

    // sincronizar a posição da nave na simulação física com a visualização
    b2Vec2 position = shipBody->GetPosition();

    _ny = position.y;
    _nx = position.x;

    shipAngle = shipBody->GetAngle() * RAD_TO_GRAUS;

    //if (value % 4 == 0)
        glutPostRedisplay();

    // registrar esse mesmo callback novamente. ele deverá ser chamado
    // repetidamente.
    glutTimerFunc(worldStep, AtualizarMundo, value+1);
}

void InicializaFisica()
{
    // o chão
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, -10.0f);

    groundBody = world.CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(1000.0f, 10.0f);
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
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    shipBody->CreateFixture(&fixtureDef);

    // definir um tanque de combustível
    tanque = new TanqueCombustivel(1000);
    motorPrincipal.setTanque(tanque);
    motorLatDir.setTanque(tanque);
    motorLatEsq.setTanque(tanque);

    // instalar um timer para atualizar o mundo
    glutTimerFunc(worldTimeStep * 1000, AtualizarMundo, 0);

    // instalar o listener de contato para detectar o 
    // momento do pouso
    world.SetContactListener(&listenerDeContato);
}

/* +---------------------------------------------------------------------+
 * |                          Funções de desenho                         |
 * +---------------------------------------------------------------------+
 */
void EscreveStatus(void)
{
    char texto[40];
    glColor3f(1,1,1);
    glRasterPos2f(5, 7);
    sprintf(texto,"x : %.3f", _nx);
    Escreva(texto);
    glRasterPos2f(5, 5);
    sprintf(texto,"Y : %.3f", _ny);
    Escreva(texto);

    if (tanque != NULL) {
        printf("Combustível: %.4f\n", tanque->combustivel());
    }
}

void DesenhaApollo11(void)
{
    glPushMatrix();
        glTranslatef(_nx, _ny, 0);
        glRotatef(shipAngle, 0, 0, 1);
        EscreveStatus();
        DesenhaObjeto(apollo11);
    glPopMatrix();
}

void DesenhaMapa(void)
{
    DesenhaObjeto(mapa);

}

void DesenhaFundo(void)
{
    glPushMatrix();
        glTranslatef(0, -80, -40);
        glEnable(GL_TEXTURE_2D); 
        glNormal3f(0,1,0);
	    glBegin(GL_QUADS);		
	        glTexCoord2f(0, 1); // canto superior esquerdo
            glVertex3f(-200, 400, 0); // canto superior esquerdo
            glTexCoord2f(0, 0); // canto inferior esquerdo
            glVertex3f(-200, 0, 15); // canto inferior esquerdo
            glTexCoord2f(1, 0); // canto inferior direito
            glVertex3f(200, 0, 15); // canto inferior direito
            glTexCoord2f(1, 1); // canto superior direito
            glVertex3f(200, 400, 0); // canto superior direito	
	    glEnd();
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

/* +---------------------------------------------------------------------+
 * |                            Laço de desenho                          |
 * +---------------------------------------------------------------------+
 */

void Desenha(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DesenhaMapa();

    DesenhaFundo();    

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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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
}

void InicializaLoaders(void) {
    // Carrega o objeto 3D
    apollo11 = CarregaObjeto("nave/apollo11/apollo11.obj", false);

    mapa = CarregaObjeto("mapas/level_1/level_1.obj", false);

    texFundo = CarregaTextura("fundo/bg.jpg", false);
    // Seta o modo de desenho
    SetaModoDesenho('s');	// 's' para sólido
}

void Inicializa(void)
{ 
    _nx = 0;
    _ny = 80;
    camOpeningAngle=45;
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

    glutIgnoreKeyRepeat(true);
    glutSpecialFunc(TeclasEspeciais);
    glutSpecialUpFunc(SpecialKeyUp);

    glutMouseFunc(GerenciaMouse);
    Inicializa();
    glutMainLoop();
}

