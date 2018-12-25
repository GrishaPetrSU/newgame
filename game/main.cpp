#include <iostream> 
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "map.h" //���������� ��� � ������
#include <list>

using namespace sf;

////////////////////////////����� ��������////////////////////////
class Entity {
public:
	enum { left, right, up, down, stay} state;// ��� ������������ - ��������� �������
	float dx, dy, x, y, speed, moveTimer;//�������� ���������� ������ ��� ������� �����
	int w, h, health, crystal; //���������� �health�, �������� ����� ������
	bool life; //���������� �life� �����, ����������
	Texture texture;//���� ��������
	Sprite sprite;//���� ������ 
	float CurrentFrame;//������ ������� ����
	std::string name;//����� ����� ���� ������, ������ ����� ��������� �� ������ //������� ����� ���� ���� �������� � update() � ����������� �� �����

	Entity(Image &image, float X, float Y, int W, int H, std::string Name){
		x = X; y = Y; //���������� ��������� �������
		w = W; h = H; 
		name = Name; 
		moveTimer = 0;
		dx = 0; dy = 0; 
		speed = 0;

		CurrentFrame = 0;
		health = 100;
		life = true; //���������������� ���������� ���������� �����, ����� ���
		texture.loadFromImage(image); //������� ���� ����������� � ��������
		sprite.setTexture(texture); //�������� ������ ���������
	}

	FloatRect getRect(){
		return FloatRect(x, y, w, h);
	}
	/*��� ������ (�����) "sf::FloatRect" ��������� ������� ������ ���������� ��������������//� ����� ���� ��� ���������� �������� ������������ ����� �� �����
	����� ��� �������� ��������, ���� �� ��� �����-���� ���� �� ���� ����� //��� �-��� ����� ��� �������� ����������� */
	virtual void update(float time) = 0;
};

////////////////////////////����� ������////////////////////////
class Player :public Entity {
public:
	int playerScore;//��� ���������� ����� ���� ������ � ������

	Player(Image &image, float X, float Y, int W, int H, std::string Name) :Entity(image, X, Y, W, H, Name){
		playerScore = 0; 
		state = stay;
		if (name == "Player"){ //������ ������� ���� ������������� ��� //������ ������ ������. IntRect � ��� ���������� �����
			sprite.setTextureRect(IntRect(0, 0, w, h));
		}
	}

	void control(){
		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			state = left;
			speed = 0.2;
		}
		if (Keyboard::isKeyPressed(Keyboard::Right)) {
			state = right;
			speed = 0.2;
		}

		if (Keyboard::isKeyPressed(Keyboard::Up)) {
			state = up;
			speed = 0.2;
		}

		if (Keyboard::isKeyPressed(Keyboard::Down)) {
			state = down;
			speed = 0.2;
		}
	}
	
//����� �������� ������������ � ���������� �����
void checkCollisionWithMap(float Dx, float Dy)	{
	for (int i = y / 32; i < (y + h) / 32; i++)//���������� �� ��������� �����
		for (int j = x / 32; j<(x + w) / 32; j++)
		{
			if (TileMap[i][j] == '0')//���� ������� ������ �����
			{
			if (Dy > 0) { y = i * 32 - h;  dy = 0; }//�� Y 
			if (Dy < 0) { y = i * 32 + 32; dy = 0; }//������������ � �������� ������ 
			if (Dx > 0) { x = j * 32 - w; dx = 0; }//� ������ ����� �����
			if (Dx < 0) { x = j * 32 + 32; dx = 0; }// � ����� ����� �����
				}

				if (TileMap[i][j] == 'h') {
					health += 20;//���� ����� ��������
					TileMap[i][j] = ' ';//������ ��������
				}
				if (TileMap[i][j] == 'c') {
					playerScore++;//���� ����� ��������
					TileMap[i][j] = ' ';//������ ��������
				}
			}
	}

void update(float time) //����� "���������/����������" ������� ������.
	{
	if (life) {//���������, ��� �� �����
		control();//������� ���������� ����������
		switch (state)//�������� ��������� �������� � ����������� �� ���������
		{
		case right:{//��������� ���� ������
			dx = speed;
			CurrentFrame += 0.005*time;
			if (CurrentFrame > 3) CurrentFrame -= 3;
			sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 192, 96, 96));
				break;
			}
		case left:{//��������� ���� �����
			dx = -speed;
			CurrentFrame += 0.005*time;
			if (CurrentFrame > 3) CurrentFrame -= 3;
			sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 96, 96, 96));
			break;
			}
		case up:{//���� �����
			dy = -speed;
			CurrentFrame += 0.005*time;
			if (CurrentFrame > 3) CurrentFrame -= 3;
			sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 288, 96, 96));
			break;
			}
		case down:{//���� ����
			dy = speed;
			CurrentFrame += 0.005*time;
			if (CurrentFrame > 3) CurrentFrame -= 3;
			sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 0, 96, 96));
			break;
			}
		case stay:{//�����
			dy = speed;
			dx = speed;
			break;
			}
			}

			x += dx*time; //�������� �� �X�
			checkCollisionWithMap(dx, 0);//������������ ������������ �� �
			y += dy*time; //�������� �� �Y�
			checkCollisionWithMap(0, dy);//������������ ������������ �� Y

			speed = 0;    //�������� ��������, ����� �������� �����������. //state = stay;
			
			sprite.setPosition(x, y); //������ � ������� (x, y).

			if (health <= 0){ life = false; }//���� ������ ������ 0, ���� ����� 0, �� ������� 
		}
	}
};


////////////////////////////����� �����////////////////////////
class Enemy :public Entity{
public:
	int direction;//����������� �������� �����
	Enemy(Image &image, float X, float Y, int W, int H, std::string Name) :Entity(image, X, Y, W, H, Name){
	if (name == "EasyEnemy"){
		//������ ������� ���� ������������� ���
		//������ ������ ������. IntRect � ��� ���������� �����
		sprite.setTextureRect(IntRect(0, 0, w, h));
		direction = rand() % (3); //����������� �������� ����� ����� ��������� �������
		//����� ��������� ��������� �����
		speed = 0.05;//���� ��������.���� ������ ������ ���������
		dx = speed;
		}
	}

void checkCollisionWithMap(float Dx, float Dy)//�-��� �������� ������������ � ������
	{
	for (int i = y / 32; i < (y + h) / 32; i++)//���������� �� ��������� �����
		for (int j = x / 32; j<(x + w) / 32; j++)
		{
			if (TileMap[i][j] == '0')//���� ������� - ������ �����
			{
				if (Dy > 0) {
					y = i * 32 - h;  dy = -0.1; 
					direction = rand() % (3); //����������� �������� �����
						}//�� Y 
				if (Dy < 0) {
					y = i * 32 + 32; dy = 0.1; 
					direction = rand() % (3);//����������� �������� ����� 
						}//������������ � �������� ������ 
				if (Dx > 0) {
					x = j * 32 - w; dx = -0.1; 
						direction = rand() % (3);//����������� �������� ����� 
						}//� ������ ����� �����
				if (Dx < 0) {
					x = j * 32 + 32; dx = 0.1; 
						direction = rand() % (3); //����������� �������� �����
						}// � ����� ����� �����
				}
			}
	}

	void update(float time)
	{
		if (name == "EasyEnemy"){//��� ��������� � ����� ������ ������ ����� �����

		if (life) {//���������, ��� �� �����
		switch (direction)//�������� ��������� �������� � ����������� �� ���������
		{
		case 0:{//��������� ���� ������
		dx = speed;
		CurrentFrame += 0.005*time;
		if (CurrentFrame > 3) CurrentFrame -= 3;
		sprite.setTextureRect(IntRect(60 * int(CurrentFrame), 120, 50, 50));
		break;
		}
		case 1:{//��������� ���� �����
		dx = -speed;
		CurrentFrame += 0.005*time;
		if (CurrentFrame > 3) CurrentFrame -= 3;
		sprite.setTextureRect(IntRect(50 * int(CurrentFrame), 60, 50, 60));
		break;
		}
		case 2:{//���� �����
		dy = -speed;
		CurrentFrame += 0.005*time;
		if (CurrentFrame > 3) CurrentFrame -= 3;
		sprite.setTextureRect(IntRect(50 * int(CurrentFrame), 180, 50, 60));
		break;
		}
		case 3:{//���� ����
		dy = speed;
		CurrentFrame += 0.005*time;
		if (CurrentFrame > 3) CurrentFrame -= 3;
		sprite.setTextureRect(IntRect(50 * int(CurrentFrame), 0, 50, 60));
		break;
		}
		}

		x += dx*time; //�������� �� �X�
		checkCollisionWithMap(dx, 0);//������������ ������������ �� �

		y += dy*time; //�������� �� �Y�
		checkCollisionWithMap(0, dy);//������������ ������������ �� Y

		sprite.setPosition(x, y); //������ � ������� (x, y).

		if (health <= 0){ life = false; }//���� ������ ������ 0, ���� ����� 0, �� �������		
		}
		}
	}
};//����� Enemy ������


////////////////////////////����� ����////////////////////////
class Bullet :public Entity{//����� ����
public:
	int direction;//����������� ����.�� ��� ��, ������ ����� � ����� ��������� ������ (int dir) ��� ������� ����������� ����� ����
	Bullet(Image &image, float X, float Y, int W, int H, std::string  Name, int dir) :Entity(image, X, Y, W, H, Name){
		x = X;
		y = Y;
		direction = dir;
		speed = 0.6;
		w = h = 16;
		life = true;
		//���� ������������� � ������������
	}

	void update(float time)
	{
		switch (direction)
		{
		case 0: dx = -speed; dy = 0;   break;// state = left
		case 1: dx = speed; dy = 0;   break;// state = right
		case 2: dx = 0; dy = -speed;   break;// state = up
		case 3: dx = 0; dy = speed;   break;// state = down
		}

		if (life){
			x += dx*time;//���� �������� ���� �� �
			y += dy*time;//�� �
			if (x <= 0) x = 20;// �������� ���� � ����� �����, ����� ��� ���������� ������ ��� �������� �� �������� �� ������ ����� � �� ���� ������ (������ ����� ���������!)
			if (y <= 0) y = 20;
			if (x >= 800) x = 780;// �������� ���� � ������ �����, ����� ��� ���������� ������ ��� �������� �� �������� �� ������ ����� � �� ���� ������ (������ ����� ���������!)
			if (y >= 640) y = 620;

			for (int i = y / 32; i < (y + h) / 32; i++)//���������� �� ��������� �����
				for (int j = x / 32; j < (x + w) / 32; j++)
				{
					if (TileMap[i][j] == '0')//���� ������� ��� ������ �����, ��
						life = false;// �� ���� �������
				}
			sprite.setPosition(x + w / 0.5, y + h / 0.35);//�������� ������� ����
		}
	}
};

////////menu
void menu(RenderWindow & window) {
	Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
	menuTexture1.loadFromFile("images/new_game.png");
	menuTexture2.loadFromFile("images/about_game.png");
	menuTexture3.loadFromFile("images/exit.png");
	aboutTexture.loadFromFile("images/verdont.png");
	menuBackground.loadFromFile("images/menu.png");
	Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), about(aboutTexture), menuBg(menuBackground);
	bool isMenu = 1;
	int menuNum = 0;
	menu1.setPosition(100, 30);
	menu2.setPosition(100, 90);
	menu3.setPosition(100, 150);
	menuBg.setPosition(345, 0);
 
	//////////////////////////////����///////////////////
	while (isMenu)
	{
		menu1.setColor(Color::White);
		menu2.setColor(Color::White);
		menu3.setColor(Color::White);
		menuNum = 0;
		window.clear(Color(129, 181, 221));
 
		if (IntRect(100, 30, 300, 50).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Blue); menuNum = 1; }
		if (IntRect(100, 90, 300, 50).contains(Mouse::getPosition(window))) { menu2.setColor(Color::Blue); menuNum = 2; }
		if (IntRect(100, 150, 300, 50).contains(Mouse::getPosition(window))) { menu3.setColor(Color::Blue); menuNum = 3; }
 
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) isMenu = false;//���� ������ ������ ������, �� ������� �� ���� 
			if (menuNum == 2) { window.draw(about); window.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
			if (menuNum == 3)  { window.close(); isMenu = false; }
		}
		window.draw(menuBg);
		window.draw(menu1);
		window.draw(menu2);
		window.draw(menu3);
		window.display();
	}
	////////////////////////////////////////////////////
}

//��������� ����
bool isGameStart() {
	
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(sf::VideoMode(800, 600, desktop.bitsPerPixel), "Skeleton");

	menu(window); //menu

	Font font;//����� 
	font.loadFromFile("ttf/CyrilicOld.ttf");//�������� ������ ������ ���� ������
	Text text("", font, 20);//������� ������ �����
	text.setColor(Color::Green);//��������� ����� � color	text.setStyle(Text::Bold);//������ �����.

	//�� ��������
	SoundBuffer shootBuffer;//������ ����� ��� �����
	shootBuffer.loadFromFile("audio/shoot.ogg");//��������� � ���� ����
	Sound shoot(shootBuffer);//������� ���� � ��������� � ���� ���� �� ������
	shoot.setVolume(20);

	SoundBuffer g_oBuffer;//������ ����� ��� �����
	g_oBuffer.loadFromFile("audio/g_o.ogg");//��������� � ���� ����
	Sound g_o(g_oBuffer);//������� ���� � ��������� � ���� ���� �� ������
	g_o.setVolume(20);

	Music music;//������� ������ ������
	music.openFromFile("audio/music.ogg");//��������� ����
	music.setVolume(30.f);
	music.play();//������������� ������
	music.setLoop(true);

	//steps sounds
	SoundBuffer stepsBuffer;//������ ����� ��� �����
	stepsBuffer.loadFromFile("audio/steps.ogg");//��������� � ���� ����
	Sound steps(stepsBuffer);//������� ���� � ��������� � ���� ���� �� ������

	Image map_image;//������ ����������� ��� �����
	map_image.loadFromFile("images/map.png");//��������� ���� ��� �����
	Texture map;//�������� �����
	map.loadFromImage(map_image);//�������� �������� ���������
	Sprite s_map;//������ ������ ��� �����
	s_map.setTexture(map);//�������� �������� ��������

	Clock clock;
	Clock gameTimeClock;//���������� �������� �������, ����� ����� ������� ����� ���� 
	int gameTime = 0;//�������� ������� �����, ����������������.

	Image heroImage;
	heroImage.loadFromFile("images/hero.png"); // ��������� ����������� ������

	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images/skel.png"); // ��������� ����������� �����

	Image BulletImage;//����������� ��� ����
	BulletImage.loadFromFile("images/bullet.png");//��������� �������� � ������ �����������

	Player p(heroImage, 100, 170, 96, 96, "Player");//������ ������ ������

	//std::list<Entity*>  enemies; //������ ������
	//std::list<Entity*>  Bullets; //������ ����
	std::list<Entity*>  entities;	//��������
	std::list<Entity*>::iterator it; //�������� ����� ��������� �� ��������� ������
	std::list<Entity*>::iterator it2;

	const int ENEMY_COUNT = 2;	//������������ ���������� ������ � ����
	int enemiesCount = 0;	//������� ���������� ������ � ����

	//��������� ������ ��������� �������
	for (int i = 0; i < ENEMY_COUNT; i++)
	{
	float xr = 150 + rand() % 500; // ��������� ���������� ����� �� ���� ���� �� ��� �x�
	float yr = 150 + rand() % 350; // ��������� ���������� ����� �� ���� ���� �� ��� �y�
	//������� ������ � �������� � ������
	entities.push_back(new Enemy(easyEnemyImage, xr, yr, 96, 96, "EasyEnemy"));
	enemiesCount += 1; //��������� ������� ������
	}

	int createObjectForMapTimer = 0;//���������� ��� ����� ��� ������������� ������

while (window.isOpen())
{
	float time = clock.getElapsedTime().asMicroseconds();

	if (p.life) gameTime = gameTimeClock.getElapsedTime().asSeconds();//������� ����� � 
		//�������� ��� ������, ���� ��� �����. ������������� ��� time ��� �� ����. ��� �� ��������� ������ ����
		clock.restart();
		time = time / 1100;
		createObjectForMapTimer += time;//���������� ������
		
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			//�������� �� ������� ������� "P"
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::P)
				{
					entities.push_back(new Bullet(BulletImage, p.x, p.y, 16, 16, "Bullet", p.state));
					shoot.play();//������ ���� ����
				}
			}
		}

		//game restart
		if (Keyboard::isKeyPressed(Keyboard::Tab)) { return true; }//���� ���, �� ������������� ����
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { return false; }//���� ������, �� ������� �� ����

		p.update(time); //�������� ������ �p� ������ �Player� 

		//�������� ������
		//for (it = entities.begin(); it != entities.end(); it++)
		//{
	//		(*it)->update(time); //��������� ����� update()
		//}

		//�������� ����
		//for (it = entities.begin(); it != entities.end(); it++)
		//{
		//	(*it)->update(time); //��������� ����� update()
		//}

		//��������� ������ �� ������� "�������" ���� � ������� ��
		for (it = entities.begin(); it != entities.end();)//������� ��� ���������� �� ������ �� �����
		{// ���� ���� ������ �����, �� ������� ���
		if ((*it)-> life == false)	{ it = entities.erase(it); } 
			else  it++;//� ���� �������� (����������) � ���� �������.		
		}

	//�������� ����������� ������ � �������.���� ����������� ���������, �� "health = 0", ����� ��������������� � ��������� ��������� "you are lose"
	if (p.life == true){//���� ����� ���
		for (it = entities.begin(); it != entities.end(); it++){//����� �� ������ ������
		if ((p.getRect().intersects((*it)->getRect())) && ((*it)->name == "EasyEnemy"))
				{
					p.health = 0;
					std::cout << "you are lose";
					p.life == false;
					music.setVolume(0);
					g_o.setVolume(80);
					g_o.play();
				}
			}
		}
	
		for (it = entities.begin(); it != entities.end();)//������� ��� ���������� �� ������ �� �����
		{
			Entity *b = *it;//��� ��������, ����� �� ������ (*it)->
			b->update(time);//�������� �-��� update ��� ���� �������� (�� ���� ��� ���, ��� ���)
			if (b->life == false)	{ it = entities.erase(it); delete b; }// ���� ���� ������ �����, �� ������� ���
			else it++;//� ���� �������� (����������) � ���� �������. ��� ������ �� ����� ��������� ������
		}

		/*//��������������
		for (it = entities.begin(); it != entities.end(); it++)//���������� �� ��-��� ������
		{
			if ((*it)->name == "EasyEnemy")
			{
				Entity *enemy = *it;
				for (std::list<Entity*>::iterator it2 = entities.begin(); it2 != entities.end(); it2++)
				{
					Entity *bullet = *it2;
					if (bullet->name == "Bullet")
						if (bullet->life == true)
						{
							if (bullet->getRect().intersects(enemy->getRect()))
							{
								bullet->life = false;
								enemy->life = false;
							}
						}
				}
			}
		}
		*/
		window.clear();

/////////////////////////////������ �����/////////////////////
for (int i = 0; i < HEIGHT_MAP; i++)
	for (int j = 0; j < WIDTH_MAP; j++)
	{
		if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
		if (TileMap[i][j] == 'c')  s_map.setTextureRect(IntRect(96, 0, 32, 32));
		if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
		if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));//������
		s_map.setPosition(j * 32, i * 32);
		window.draw(s_map);
	}

		//�������� ���������� �������� � �������
		std::ostringstream playerHealthString, gameTimeString, gameCrystal;

		playerHealthString << p.health; 
		gameTimeString << gameTime;//��������� ������
		gameCrystal << p.playerScore;
		text.setString("��������: " + playerHealthString.str() + "\n����� ����: " + gameTimeString.str() + "\n��������� " + gameCrystal.str());//������ ������ ������
		text.setPosition(50, 50);//������ ������� ������
		window.draw(text);//������ ���� �����

		//������ ������
		for (it = entities.begin(); it != entities.end(); it++)
		{
			if ((*it)->life) //���� ����� ����
			window.draw((*it)->sprite); //������ 
		}

		//������ ����
		for (it = entities.begin(); it != entities.end(); it++)
		{
			if ((*it)->life) //���� ���� ����
			window.draw((*it)->sprite); //������ �������
		}

		window.draw(p.sprite);//������ ������ ������� �p� ������ �Player�
		window.display();
	}
};

void gameRunning(){//�-��� ������������� ���� , ���� ��� ����������
	if (isGameStart()) { gameRunning(); }////���� startGame() == true, �� �������� ������ �-��� isGameRunning, ������� � ���� ������� ����� �������� startGame() 
}

int main()
{
	gameRunning();//��������� ������� ����
	return 0;
}