#include "stdafx.h"
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//hace el codigo mas facil de escribir
using namespace sf;

void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite Branches[NUM_BRANCHES];
//donde esta el jugador y la rama
//IZQUIERDA, DERECHA	
enum class side { LEFT, RIGHT, NONE };//3 posibles resultados
side branchPositions[NUM_BRANCHES];

int main() {
	//tama;o de la pantalla por pixeles alto y largo
	VideoMode vm(1920, 1080);

	//crea y abre una ventana para el juego
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

	//crear una textura que sostenga graficos en la gpu
	Texture textureBackground;

	//cargar un grafico en la textura
	textureBackground.loadFromFile("graphics/background.png");

	//crear un sprite
	Sprite spriteBackground;

	//adjuntar una textura a un sprite
	spriteBackground.setTexture(textureBackground);

	//configurar el spriteBackgroun a la screen
	spriteBackground.setPosition(0, 0);

	//realizar un arbol sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	//preparar la abeja
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	//esta la abeja moviendose?
	bool beeActive = false;
	//que tan rapido puede volar la abeja
	float beeSpeed = 0.0f;

	//hacer 3 nubes sprites desde una textura
	Texture textureCloud;
	// cargar una nueva textura
	textureCloud.loadFromFile("graphics/cloud.png");
	// 3 nuevos sprite con la misma textura
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);

	//posicionar las nubes en la pantalla
	spriteCloud1.setPosition(0, 200);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);

	//estan las nubes en la pantalla?
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;
	// que tan rapido van las nubes?
	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	//variable para controlar el tiempo
	Clock clock;

	//time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;



	//rastrear donde sea que el juego este corriendo
	bool paused = true;
	//dibujar texto
	int score = 0;
	sf::Text messageText;
	sf::Text scoreText;

	//escojer una fuente
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	//configurar la fuente de nuestro mensaje
	messageText.setFont(font);
	scoreText.setFont(font);

	//asignar un mensaje actual
	messageText.setString("Press enter to start!");
	scoreText.setString("Score = 0");

	//hacerlo realidad
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	//escojer un color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	//posicionando el texto
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left +
		textRect.width / 2.0f,
		textRect.top +
		textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	// preparar 6 branches
	Texture textureBranches;
	textureBranches.loadFromFile("graphics/branch.png");
	//configurar la textura por cada branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {

		Branches[i].setTexture(textureBranches);
		Branches[i].setPosition(-2000, -2000);
		//configurar el origen del sprite al centro
		//podemos girarlos sin cambiarlos de posicion
		Branches[i].setOrigin(220, 20);
	}
	//preparar el jugador
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	//el jugador empieza en el lado izquierdo
	side playerSide = side::LEFT;

	//preparar la lapida
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	//preparar la hacha
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	//poner la hacha en fila con el arbol
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	//preparar el tronco volador
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	//otros troncos funcionales relacionados a variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;
	//control de el jugador
	bool acceptInput = false;

	//preparar el sonido

	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	//fuera de tiempo
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);


	while (window.isOpen()) {

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused)
			{
				acceptInput = true;

				//esconde la hacha
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}
		//empezar el juego
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			//resetear el tiempo y el score
			score = 0;
			timeRemaining = 5;

			//hacer todos los branches desaparecer 
			for (int i = 1; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;

			}
			//asegurarse que la lapida este escondida
			spriteRIP.setPosition(675, 2000);

			//mover el jugador hacia la posicion
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}
		//envolver los controles del jugador
		//asegurarse que estamos aceptanddo input
		if (acceptInput) {

			if (Keyboard::isKeyPressed(Keyboard::Right)) {

				//asegurarse que el jugador este en la derecha
				playerSide = side::RIGHT;
				score++;

				//agregar la cantidad de tiempo restante
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_RIGHT,
					spriteAxe.getPosition().y);

				spritePlayer.setPosition(1200, 720);

				//actualizar las ramas
				updateBranches(score);

				// configurar los troncos flotantes a la izquierda
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;
				//sonido
				chop.play();
			}
		}
			//manejar el click izquierdo
			if (acceptInput) {

				if (Keyboard::isKeyPressed(Keyboard::Left)) {

					//asegurarse que el jugador este en la derecha
					playerSide = side::LEFT;
					score++;

					//agregar la cantidad de tiempo restante
					timeRemaining += (2 / score) + .15;

					spriteAxe.setPosition(AXE_POSITION_LEFT,
						spriteAxe.getPosition().y);

					spritePlayer.setPosition(580, 720);

					//actualizar las ramas
					updateBranches(score);

					// configurar los troncos flotantes a la izquierda
					spriteLog.setPosition(810, 720);
					logSpeedX = 5000;
					logActive = true;

					acceptInput = false;
					//sonido
					chop.play();
				}

			}

			if (!paused) {


				// medida de tiempo
				Time dt = clock.restart();

				//sustraer desde la cantidad  de tiempo restante
				timeRemaining -= dt.asSeconds();
				//incremente el time bar
				timeBar.setSize(Vector2f(timeBarWidthPerSecond *
					timeRemaining, timeBarHeight));

				if (timeRemaining <= 0.0f) {
					//pausa el juego
					paused = true;

					//cambiar el mensaje mostrado al jugador
					messageText.setString("Tiempo terminado!!");

					//repocisionar el texto basado en el nuevo tama;o
					FloatRect textRect = messageText.getLocalBounds();
					messageText.setOrigin(textRect.left +
						textRect.width / 2.0f,
						textRect.top +
						textRect.height / 2.0f);
					messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
					//musica de final de juego
					outOfTime.play();
				}
				//configurar la abeja
				if (!beeActive) {
					//que tan rapida is la abeja
					srand((int)time(0) * 10); //obtener un numero random
					beeSpeed = (rand() % 200) + 200; // entre 199 a 399

					//que tan alto vuela la abeja
					srand((int)time(0) * 10);
					float height = (rand() % 500) + 500;
					spriteBee.setPosition(2000, height);
					beeActive = true;
				}
				else {
					// mover la abeja
					spriteBee.setPosition(
						spriteBee.getPosition().x -
						(beeSpeed * dt.asSeconds()),
						spriteBee.getPosition().y);

					//la abeja a alcanzado el borde derecho de la pantalla?
					if (spriteBee.getPosition().x < -100) {
						//configurarlo listo para ser completamente una nueva nube en el siguiente frame
						beeActive = false;
					}
				}
				if (!cloud1Active) {

					//que tan rapido va la nube
					srand((int)time(0) * 10);
					cloud1Speed = (rand() % 200);

					//que tan alto vuela la nube
					srand((int)time(0) * 10);
					float height = (rand() % 150);
					spriteCloud1.setPosition(-200, height);
					cloud1Active = true;
				}
				else {

					spriteCloud1.setPosition(
						spriteCloud1.getPosition().x +
						(cloud1Speed * dt.asSeconds()),
						spriteCloud1.getPosition().y);

					//la nube a alcanzado el vorde derecho de la pantalla?
					if (spriteCloud1.getPosition().x > 1920) {
						//configurarlo listo para que sea completamente una nueva nube en el siguiente fram
						cloud1Active = false;
					}
				}
				if (!cloud2Active) {
					//que tan rapido va la nube
					srand((int)time(0) * 20);
					cloud2Speed = (rand() % 200);

					//que tan alto vuela la nube
					srand((int)time(0) * 20);
					float height = (rand() % 300) - 150;
					spriteCloud2.setPosition(-200, height);
					cloud2Active = true;
				}
				else {
					spriteCloud2.setPosition(
						spriteCloud2.getPosition().x +
						(cloud2Speed * dt.asSeconds()),
						spriteCloud2.getPosition().y);

					//la nube a alcanzado el vorde derecho de la pantalla?
					if (spriteCloud2.getPosition().x > 1920) {
						//configurarlo listo para que sea completamente una nueva nube en el siguiente fram
						cloud2Active = false;
					}

				}
				if (!cloud3Active) {
					//que tan rapido va la nube
					srand((int)time(0) * 30);
					cloud3Speed = (rand() % 200);

					//que tan alto vuela la nube
					srand((int)time(0) * 30);
					float height = (rand() % 450) - 150;
					spriteCloud3.setPosition(-200, height);
					cloud3Active = true;
				}
				else {
					spriteCloud3.setPosition(
						spriteCloud3.getPosition().x +
						(cloud3Speed * dt.asSeconds()),
						spriteCloud3.getPosition().y);

					//la nube a alcanzado el vorde derecho de la pantalla?
					if (spriteCloud3.getPosition().x > 1920) {
						//configurarlo listo para que sea completamente una nueva nube en el siguiente fram
						cloud3Active = false;
					}
				}
				//actualizar el score text
				std::stringstream ss;
				ss << "Score = " << score;
				scoreText.setString(ss.str());

				//actualizar los branch sprites
				for (int i = 0; i < NUM_BRANCHES; i++) {
					float height = i * 150;//configurando posiciones de cada branch
					if (branchPositions[i] == side::LEFT) {
						//mover el sprite al lado izquierdo
						Branches[i].setPosition(610, height);
						//girar el sprite al otro lado
						Branches[i].setRotation(180);
					}
					else if (branchPositions[i] == side::RIGHT) {
						//mover el sprite al lado izquierdo
						Branches[i].setPosition(1330, height);
						//configurar la rotacion normal del sprite 
						Branches[i].setRotation(0);
					}
					else {
						// esconder el branch
						Branches[i].setPosition(3000, height);
					}
					//manejar el tronco volador
					if (logActive) {
						spriteLog.setPosition(
							spriteLog.getPosition().x +
							(logSpeedX * dt.asSeconds()),
							spriteLog.getPosition().y +
							(logSpeedY * dt.asSeconds()));
						//a alcanzado el tronco el borde izquierdo
						if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000) {
							//configurarlo para todo el nuevo y siguiente tronco frame
							logActive = false;
							spriteLog.setPosition(810, 720);
						}
					}

				}
				//el jugador a sido aplastado por una rama
				if (branchPositions[5] == playerSide) {
					//m,uerto
					paused = true;
					acceptInput = false;

					//dibujar la lapida
					spriteRIP.setPosition(525, 760);

					//esconder el jugador
					spritePlayer.setPosition(2000, 660);

					//cambiar el mensaje 
					messageText.setString("Aplastado!!!!!");

					//centrar el mensaje
					FloatRect textRect = messageText.getLocalBounds();

					messageText.setOrigin(textRect.left +
						textRect.width / 2.0f,
						textRect.top + textRect.height / 2.0f);

					messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
					death.play();
				}
			}//end if paused
			//limpiar pantalla desde el ultimo fotograma
			window.clear();
			//dibujar nuestra escena del juego aqui
			window.draw(spriteBackground);
			//dibujar las nubes
			window.draw(spriteCloud1);
			window.draw(spriteCloud2);
			window.draw(spriteCloud3);
			//dibujar los branches
			for (int i = 0; i < NUM_BRANCHES; i++) {
				window.draw(Branches[i]);
			}
			//dibujar el arbol
			window.draw(spriteTree);
			//dibujar el jugado
			window.draw(spritePlayer);
			//dibujar la hacha
			window.draw(spriteAxe);
			//dibujar el tronco volador
			window.draw(spriteLog);
			//dibujar la lapida
			window.draw(spriteRIP);
			//dibujar el insecto
			window.draw(spriteBee);
			//dibujar el score
			window.draw(scoreText);
			//dibujar timebar
			window.draw(timeBar);

			if (paused) {
				window.draw(messageText);
			}
			//muestra todo lo que hemos dibujado
			window.display();
		}
		return 0;
	}


void updateBranches(int seed) {
	//mover los branches un lugar abajo
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}
	//spawn un nuevo branch en la posicion 0
//LEFT, RIGHT, NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;
	}
}