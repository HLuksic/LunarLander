#define OLC_PGE_APPLICATION

#include <iostream>
#include <random>
#include <ctime>
#include <deque>
#include <algorithm>

#include "olcPixelGameEngine.h"

constexpr float PI = 3.14159f;
constexpr float PI2 = PI/2;

// TODO: 
// FOREGROUND
// BLACK DECALS FOR TERRAIN
// STARS
// DEBRIS PARTICLES, 
// SOUNDS,
//  
// HIGH SCORE, 
// ADD DIFFICULTY (choose with numpad in title)

class lunarLander : public olc::PixelGameEngine
{
public:
	lunarLander()
	{
		sAppName = "Lunar lander";
	}

private:
	int screen_height;
	int screen_width;

	float player_angle;
	float player_altitude;
	float player_thrust;
	bool  player_dead;
	float player_score;
	float player_fuel;
	int	  player_landings;
	float scale;
	float current_seg_angle;
	int   normHVel;
	int   normVVel;
	bool  title;
	bool  pause;

	std::unique_ptr<olc::Sprite> sprPlayer;
	std::unique_ptr<olc::Sprite> sprPlayerLightDamage;
	std::unique_ptr<olc::Sprite> sprPlayerMediumDamage;
	std::unique_ptr<olc::Sprite> sprPlayerHeavyDamage;
	std::unique_ptr<olc::Sprite> sprPlayerDestroyed;
	std::unique_ptr<olc::Sprite> sprBurnerEnd;
	std::unique_ptr<olc::Sprite> sprEnd;
	std::unique_ptr<olc::Sprite> sprEarth;
	std::unique_ptr<olc::Sprite> sprSurface;

	std::unique_ptr<olc::Decal> decPlayer;
	std::unique_ptr<olc::Decal> decPlayerLightDamage;
	std::unique_ptr<olc::Decal> decPlayerMediumDamage;
	std::unique_ptr<olc::Decal> decPlayerHeavyDamage;
	std::unique_ptr<olc::Decal> decPlayerDestroyed;
	std::unique_ptr<olc::Decal> decBurnerEnd;
	std::unique_ptr<olc::Decal> decEnd;
	std::unique_ptr<olc::Decal> decSurface;

	olc::vf2d player_pos;
	olc::vf2d player_vel;
	olc::vf2d earth_pos;
	olc::vf2d adj_pos;

	struct sSegment {
		olc::vf2d leftnode;
		olc::vf2d rightnode;
		float	  angle;
		bool	  visited;
	};

	struct sParticle {
		olc::vf2d position;
		float	  angle;
		float	  life;
	};

	std::deque<sSegment>  deqSegments;
	std::deque<sSegment>  deqBgSegments;
	std::deque<sSegment>  deqFgSegments;
	std::vector<sSegment> vecParticles;

public:
	bool OnUserCreate() override
	{
		srand((unsigned int)time(0));

		screen_height = ScreenHeight();
		screen_width  = ScreenWidth();

		// Player
		player_angle    = 0.0f;
		player_pos      = { (float)screen_width / 2, (float)screen_height / 2 };
		player_vel      = { 0.0f, 0.7f };
		player_thrust   = 0.0f;
		player_score    = 0;
		player_fuel     = 2250;
		player_landings = 0;
		player_dead     = false;
		pause			= false;
		title			= true;

		// Environment
		adj_pos   = player_pos / 2;
		scale     = 0.5f;
		earth_pos = { rand_float(10.0f, screen_width - 10.0f), rand_float(10.0f, screen_height / 3) };

		// Sprites
		sprPlayerLightDamage  = std::make_unique<olc::Sprite>("C:/Users/Admin/Desktop/ConsoleApplication3/landerLightDamage.png");
		sprPlayerMediumDamage = std::make_unique<olc::Sprite>("C:/Users/Admin/Desktop/ConsoleApplication3/landerMediumDamage.png");
		sprPlayerHeavyDamage  = std::make_unique<olc::Sprite>("C:/Users/Admin/Desktop/ConsoleApplication3/landerHeavyDamage.png");
		sprPlayerDestroyed    = std::make_unique<olc::Sprite>("C:/Users/Admin/Desktop/ConsoleApplication3/landerDestroyed.png");
		sprPlayer             = std::make_unique<olc::Sprite>("C:/Users/Admin/Desktop/ConsoleApplication3/lander.png");
		sprBurnerEnd		  = std::make_unique<olc::Sprite>("C:/Users/Admin/Desktop/ConsoleApplication3/burner.png");
		sprEnd				  = std::make_unique<olc::Sprite>("C:/Users/Admin/Desktop/ConsoleApplication3/end.png");
		sprEarth			  = std::make_unique<olc::Sprite>("C:/Users/Admin/Desktop/ConsoleApplication3/earth.png");
		sprSurface			  = std::make_unique<olc::Sprite>("C:/Users/Admin/Desktop/ConsoleApplication3/ground.png");

		// Decals
		decPlayerLightDamage  = std::make_unique<olc::Decal>(sprPlayerLightDamage.get());
		decPlayerMediumDamage = std::make_unique<olc::Decal>(sprPlayerMediumDamage.get());
		decPlayerHeavyDamage  = std::make_unique<olc::Decal>(sprPlayerHeavyDamage.get());
		decPlayerDestroyed	  = std::make_unique<olc::Decal>(sprPlayerDestroyed.get());
		decPlayer			  = std::make_unique<olc::Decal>(sprPlayer.get());
		decBurnerEnd		  = std::make_unique<olc::Decal>(sprBurnerEnd.get());
		decEnd				  = std::make_unique<olc::Decal>(sprEnd.get());
		decSurface			  = std::make_unique<olc::Decal>(sprSurface.get());
		
		return true;
	}

	float rand_float(float a, float b)
	{
		return ((b - a) * ((float)rand() / RAND_MAX)) + a;
	}

	float get_ground_angle(olc::vf2d node1, olc::vf2d node2) 
	{
		return atan2(node2.y - node1.y, node2.x - node1.x);
	}

	void debug()
	{
		// Highlight front() And back() Segment
		//DrawLine(
		//	deqSegments.front().leftnode * scale + adj_pos, 
		//	deqSegments.front().rightnode * scale + adj_pos, 
		//	olc::GREEN);
		//DrawCircle(
		//	deqSegments.front().rightnode * scale + adj_pos, 
		//	5, 
		//	olc::GREEN);
		//DrawLine(
		//	deqSegments.back().leftnode * scale + adj_pos, 
		//	deqSegments.back().rightnode * scale + adj_pos, 
		//	olc::RED);
		//DrawCircle(
		//	deqSegments.back().leftnode * scale + adj_pos, 
		//	5, 
		//	olc::RED);

		//// Highlight Segment & Point Under Player
		//for (auto& segment : deqSegments)
		//{
		//	if (segment.leftnode.x < player_pos.x && segment.rightnode.x > player_pos.x)
		//	{
		//		DrawCircle(segment.leftnode * scale + adj_pos, 5, olc::YELLOW);
		//		DrawCircle(segment.rightnode * scale + adj_pos, 5, olc::YELLOW);
		//
		//		float diffy = segment.rightnode.y - segment.leftnode.y;
		//		float diffx = segment.rightnode.x - segment.leftnode.x;
		//		float mult = (player_pos.x - segment.leftnode.x) / diffx;
		//		olc::vf2d newpoint = { player_pos.x, segment.leftnode.y + diffy * mult };
		//		DrawCircle(newpoint * scale + adj_pos, 5, olc::YELLOW);
		//	}
		//}

		//// Player Collision Circle
		//DrawCircle(player_pos * scale + adj_pos, int(7 * scale), olc::RED);

		//DrawString(50, 50, std::to_string(deqSegments.size()));
		//DrawString(60, 60, std::to_string(deqBgSegments.size()));
	}
	
	void physics(float fElapsedTime) 
	{ 
		static float Time = 0.0f;

		// Divide By 3 For Believable Velocity, This Is What's Displayed
		normHVel = abs((int)(player_vel.x / 3));
		normVVel = abs((int)(player_vel.y / 3));

		if (!pause) // Disable Everything If Paused
		{
			// Angle Reset If Full Circle
			if (abs(player_angle) > 6.283f) player_angle = 0.0f;

			// Directional Velocity
			player_vel += { player_thrust * cos(player_angle + PI * 0.5f) * fElapsedTime,
							player_thrust * sin(player_angle + PI * 0.5f) * fElapsedTime };
		
			// We're On The Moon, No Drag
			// Gravity
			player_vel.y += 15.0f * fElapsedTime;

			// 1 Score Per Second, This Is Why Score Is Float Internally
			player_score += fElapsedTime;

			// Player Is Stationary, World Moves Inversely
			for (auto& segment : deqSegments)
			{
				segment.leftnode  += player_vel * -1.0f * fElapsedTime;
				segment.rightnode += player_vel * -1.0f * fElapsedTime;
			}

			for (auto& segment : deqBgSegments)
			{
				segment.leftnode += player_vel * -0.3f * fElapsedTime;
				segment.rightnode += player_vel * -0.3f * fElapsedTime;
			}

			if (GetKey(olc::Key::W).bHeld && (int)player_fuel)
			{
				player_thrust = -40.0f;
				player_fuel  -= 100.0f * fElapsedTime;
			}
			else 
			{
				player_thrust = 0.0f;
			}

			if (GetKey(olc::Key::A).bHeld && (int)player_fuel)
			{
				player_angle -= 1.5f * fElapsedTime;
				player_fuel -= 10.0f * fElapsedTime;
			}
			if (GetKey(olc::Key::D).bHeld && (int)player_fuel)
			{
				player_angle += 1.5f * fElapsedTime;
				player_fuel -= 10.0f * fElapsedTime;
			}
		}

		// Shift To Change Zoom
		if (GetKey(olc::SHIFT).bPressed)
		{
			if (scale == 1.5f)
			{
				scale = 0.5f;
				adj_pos = player_pos * 0.5f; // To Center Everything When Zoomed Out
			}
			else
			{
				scale = 1.5f;
				adj_pos = player_pos * -0.5f;
			}
		}

		for (auto& segment : deqSegments)
		{
			// Get Segment Under Player
			if (segment.leftnode.x < player_pos.x && segment.rightnode.x > player_pos.x)
			{
				// Calculate Point Directly Under Player
				current_seg_angle = segment.angle;
				float diffy = segment.rightnode.y - segment.leftnode.y;
				float diffx = segment.rightnode.x - segment.leftnode.x;
				float mult = (player_pos.x - segment.leftnode.x) / diffx;
				// olc::vf2d newpoint = { playerPos.x, segment.leftnode.y + diffy * mult };
				// Altitude = Y Distance To newpoint, + 7 As Lower Edge Of Player Sprite
				player_altitude = ((segment.leftnode.y + diffy * mult) - (player_pos.y + 7)) / 3;

				if (player_altitude < 0.7f)
				{
					landing_handler(segment);
					break;
				}
			}
		}

	}

	void landing_handler(sSegment& segment) 
	{
		pause = true; // Pause On Ground

		bool success = normHVel <= 3 &&
					   normVVel <= 2 &&
					   abs(segment.angle) <= 0.349f && // 20 deg
					   !segment.visited &&
					   abs(player_angle - segment.angle) <= 0.087f; // 5 deg

		int vel = normHVel + normVVel;

		if (success)
		{
			switch (vel)
			{
				case 0:
					DrawString({
						int(screen_width * 0.4f), int(screen_height * 0.25f) }, 
						"PERFECT LANDING!");
					break;
				case 1:
					DrawString({ 
						int(screen_width * 0.4f), int(screen_height * 0.25f) }, 
						"Great landing!");
					break;
				case 2:
					DrawString({ 
						int(screen_width * 0.4f), int(screen_height * 0.25f) }, 
						"Good landing!");
					break;
				case 3:
					DrawString({ 
						int(screen_width * 0.4f), int(screen_height * 0.25f) }, 
						"You made it!");
					break;
				case 4:
					DrawString({ 
						int(screen_width * 0.39f), int(screen_height * 0.25f) }, 
						"Bit shaky there!");
					break;
				case 5:
					DrawString({ 
						int(screen_width * 0.39f), int(screen_height * 0.25f) }, 
						"Almost lost it!");
					break;
			}

			DrawString({ 
				int(screen_width * 0.47f), int(screen_height * 0.2f)}, 
				"+" + std::to_string(int(50 + abs(segment.angle) * 544 * (5 - vel))));

			DrawString({ 
				int(screen_width * 0.435f), int(screen_height * 0.4f) }, 
				"+500 fuel", 
				olc::DARK_GREY);

			DrawStringDecal({
				screen_width * 0.35f, screen_height * 0.8f },
				"Press SPACE to continue!");
			
			// Unpause On Space
			if (GetKey(olc::Key::SPACE).bPressed)
			{
				// Launch Based On Ground Angle
				// Add Score 50-1000 Based On Ground Angle, Velocity
				player_vel		 = { -cos(player_angle + PI2) * 60.0f, -sin(player_angle + PI2) * 60.0f };
				player_score    += int(50 + abs(segment.angle) * 544 * (5 - vel)); 
				player_fuel     += 500;
				player_landings += 1;
				segment.visited  = true;
				pause            = false;
			}
		}	
		else
		{
			player_dead = true;

			if (vel < 7)
			{
				DrawString({ 
				int(screen_width * 0.31f), int(screen_height * 0.25f) }, 
				"You broke the landing gear!");
			}
			else if (vel < 10)
			{
				DrawString({ 
				int(screen_width * 0.4f), int(screen_height * 0.25f) }, 
				"You crashed it!");
			}
			else if (vel < 20)
			{
				DrawString({ 
				int(screen_width * 0.4f), int(screen_height * 0.25f) }, 
				"You wrecked it!");
			}
			else if (vel >= 20)
			{
				DrawString({ 
				int(screen_width * 0.37f), int(screen_height * 0.25f) }, 
				"YOU BLEW A CRATER!");
			}
			else if (abs(player_angle - segment.angle) > PI2)
			{
				DrawString({ 
				int(screen_width * 0.37f), int(screen_height * 0.25f) }, 
				"You're in the dirt!");
			}

			DrawStringDecal({
				screen_width * 0.33f, screen_height * 0.8f },
				"Press SPACE to restart!");
			
			// Reset Game
			if (GetKey(olc::Key::SPACE).bPressed)
			{
				player_pos		= { (float)screen_width / 2, (float)screen_height / 2 };
				player_vel		= { 0.0f, 0.0f };
				adj_pos			= { player_pos / 2 };
				earth_pos		= { rand_float(10.0f, screen_width - 10.0f), rand_float(10.0f, screen_height / 3) };
				player_angle	= 0.0f;
				player_thrust	= 0.0f;
				player_score	= 0.0f;
				player_fuel		= 2250;
				player_landings = 0;
				player_dead		= false;
				pause			= false;
				scale			= 0.5f;

				deqSegments.clear();
				deqBgSegments.clear();
			}
		}
	}

	template<typename T>
	void create_new_segment(T scene, bool left, olc::vf2d distx, olc::vf2d disty)
	{
		// New rightnode Is The Previous back() Segments leftnode
		// New leftnode Is Random With Regard To Previous back() Segments leftnode
		if (left)
		{
			olc::vf2d newrightnode = (*scene).back().leftnode;
			olc::vf2d newleftnode = { 
				(*scene).back().leftnode.x - rand_float(distx.x, distx.y),
				(*scene).back().leftnode.y + rand_float(disty.x, disty.y) };

			(*scene).push_back({
				newleftnode,
				newrightnode,
				get_ground_angle(newleftnode, newrightnode),
				false });
		}
		else
		{
			olc::vf2d newleftnode = (*scene).front().rightnode;
			olc::vf2d newrightnode = { 
				(*scene).front().rightnode.x + rand_float(distx.x, distx.y),
				(*scene).front().rightnode.y + rand_float(disty.x, disty.y) };

			(*scene).push_front({
				newleftnode,
				newrightnode,
				get_ground_angle(newleftnode, newrightnode),
				false });
		}
	}

	void terrain() 
	{
		// Plant First Node
		if (!deqSegments.size() && !deqBgSegments.size())
		{
			olc::vf2d node1 = { 
				player_pos.x - 20.0f, 
				rand_float(screen_height * 1.3f, screen_height * 1.6f) };

			olc::vf2d node2 = { 
				player_pos.x + 20.0f, 
				rand_float(screen_height * 1.3f, screen_height * 1.6f) };

			// Actual Ground
			deqSegments.push_back({ 
				node1,
				node2,
				get_ground_angle(node1, node2),
				false });
			
			// Background
			deqBgSegments.push_back({ 
				{ player_pos.x - 7.0f, rand_float(screen_height * 0.9f, screen_height * 1.0f) },
				{ player_pos.x + 7.0f, rand_float(screen_height * 0.9f, screen_height * 1.0f) },
				0.0f,
				false });
		}
		
		// If The front() Or back() Segment Is Near The Screen Border, Spawn New Ones
		if (deqSegments.back().rightnode.x > -250)
			create_new_segment(&deqSegments, true, { 30.0f, 50.0f }, { -50.0f, 50.0f });

		if (deqSegments.front().leftnode.x < screen_width + 250)
			create_new_segment(&deqSegments, false, { 30.0f, 50.0f }, { -50.0f, 50.0f });
		
		if (deqBgSegments.back().rightnode.x > -250)
			create_new_segment(&deqBgSegments, true, { 15.0f, 25.0f }, { -15.0f, 15.0f });
		
		if (deqBgSegments.front().leftnode.x < screen_width + 250)
			create_new_segment(&deqBgSegments, false, { 15.0f, 25.0f }, { -15.0f, 15.0f });
	}

	void draw(float fElapsedTime)
	{
		static float burnertime = 0.0f;
		static float texttime = 0.0f;
		static float Time = 0.0f;
		Time += fElapsedTime;

		Clear(olc::BLACK);

		DrawSprite(
			earth_pos * scale + adj_pos, 
			sprEarth.get(), 
			int(scale + 1.0f));



		// Background
		for (auto& segment : deqBgSegments)
		{
			// Find Segments On Screen
			if (segment.rightnode.x < screen_width / scale - adj_pos.x &&
				segment.leftnode.x > -screen_width / scale + adj_pos.x)
			{
				// Fill Black
				DrawWarpedDecal(
					decSurface.get(),
					{ segment.leftnode * scale + adj_pos,
					segment.rightnode * scale + adj_pos,
					olc::vf2d(segment.rightnode.x, segment.rightnode.y + 100.0f) * scale + adj_pos,
					olc::vf2d(segment.leftnode.x, segment.leftnode.y + 100.0f) * scale + adj_pos },
					olc::VERY_DARK_GREY);
			}
		}

		// TODO: CONVERT TO VECTOR OF PAIRS
		if (!player_dead)
		{
			DrawRotatedDecal(
				player_pos * scale + adj_pos,
				decPlayer.get(),
				player_angle,
				olc::vf2d(8.0f, 8.0f),
				olc::vf2d(1.0f, 1.0f) * scale);
		}
		else
		{
			// Different Levels Of Damage Decals
			if (normHVel + normVVel < 7)
			{
				DrawRotatedDecal(
					player_pos * scale + adj_pos,
					decPlayerLightDamage.get(),
					current_seg_angle,
					olc::vf2d(8.0f, 8.0f),
					olc::vf2d(1.0f, 1.0f) * scale);
			}
			else if (normHVel + normVVel < 10)
			{
				DrawRotatedDecal(
					player_pos * scale + adj_pos,
					decPlayerMediumDamage.get(),
					current_seg_angle,
					olc::vf2d(8.0f, 8.0f),
					olc::vf2d(1.0f, 1.0f) * scale);
			}
			else if (normHVel + normVVel < 13)
			{
				DrawRotatedDecal(
					player_pos * scale + adj_pos,
					decPlayerHeavyDamage.get(),
					current_seg_angle,
					olc::vf2d(8.0f, 8.0f),
					olc::vf2d(1.0f, 1.0f) * scale);
			}
			else if (normHVel + normVVel > 12)
			{
				DrawRotatedDecal(
					player_pos * scale + adj_pos,
					decPlayerDestroyed.get(),
					current_seg_angle,
					olc::vf2d(8.0f, 8.0f),
					olc::vf2d(1.0f, 1.0f) * scale);
			}
		}
		
		if ((int)player_fuel && !pause)
		{
			if (GetKey(olc::Key::W).bHeld)
			{
				burnertime += fElapsedTime;
				if (burnertime > 1.0f) 
					burnertime = 1.0f;

				// Using sin() and Time Here For Growth Effect
				DrawRotatedDecal(
					player_pos * scale + adj_pos,
					decBurnerEnd.get(),
					player_angle,
					{ 8.0f, -8.0f },
					olc::vf2d( 0.8f + abs(sin(burnertime)) * 0.5f, 0.8f + abs(sin(burnertime)) * 0.5f ) * scale);
			}
			else
			{
				// Short Burnoff Decal, Can't Use bReleased Because Drawing For One Frame Is Too Short
				if (burnertime > 0.0f)
					DrawRotatedDecal(
						player_pos * scale + adj_pos,
						decEnd.get(),
						player_angle,
						olc::vf2d( 8.0f, -5.0f ),
						olc::vf2d( 2.0f + abs(sin(burnertime) * 3.0f), 1.5f) * scale);

				burnertime -= fElapsedTime * 10;

				if (burnertime < 0.0f) 
					burnertime = 0.0f;
			}
			// Smaller Side Thrusters
			if (GetKey(olc::Key::A).bHeld)
			{
				DrawRotatedDecal(player_pos * scale + adj_pos,
								 decBurnerEnd.get(),
								 player_angle + 0.5f * PI,
								 olc::vf2d(-25.0f, -15.0f),
								 olc::vf2d(0.2f, 0.5f) * scale);
			}
			if (GetKey(olc::Key::D).bHeld)
			{
				DrawRotatedDecal(player_pos * scale + adj_pos,
								 decBurnerEnd.get(),
								 player_angle - 0.5f * PI,
								 olc::vf2d(41.0f, -15.0f),
								 olc::vf2d(0.2f, 0.5f) * scale);
			}
		}
		
		// Terrain
		for (auto& segment : deqSegments)
		{
			if (segment.rightnode.x < screen_width / scale - adj_pos.x &&
				segment.leftnode.x > -screen_width / scale + adj_pos.x)
			{
				// Distinguish Landable Segments
				if (abs(segment.angle) <= 0.349f && !segment.visited)
				{
					DrawWarpedDecal(
						decSurface.get(),
						{ segment.leftnode * scale + adj_pos,
						segment.rightnode * scale + adj_pos,
						olc::vf2d(segment.rightnode.x, segment.rightnode.y + 500.0f) * scale + adj_pos,
						olc::vf2d(segment.leftnode.x, segment.leftnode.y + 500.0f) * scale + adj_pos },
						olc::WHITE);
				}
				else
				{
					DrawWarpedDecal(
						decSurface.get(),
						{ segment.leftnode * scale + adj_pos,
						segment.rightnode * scale + adj_pos,
						olc::vf2d(segment.rightnode.x, segment.rightnode.y + 500.0f) * scale + adj_pos,
						olc::vf2d(segment.leftnode.x, segment.leftnode.y + 500.0f) * scale + adj_pos },
						olc::DARK_GREY);
				}
			}
		}

		// UI
		if (texttime < 3.0f)
		{
			DrawString({ int(screen_width * 0.27f), int(screen_height * 0.25f)}, "Land on the highlighted segments!");
			texttime += fElapsedTime;
		}

		std::vector<std::pair<olc::vf2d, std::string>> ui = {
			{ {screen_width * 0.05f, screen_height * 0.05f}, "H.V. " + std::to_string(normHVel) + "m/s" },
			{ {screen_width * 0.05f, screen_height * 0.07f}, "V.V. " + std::to_string(normVVel) + "m/s" },
			{ {screen_width * 0.85f, screen_height * 0.05f}, "F " + std::to_string((int)player_fuel) + "kg"},
			{ {screen_width * 0.85f, screen_height * 0.07f}, "L " + std::to_string(player_landings) },
			{ {screen_width * 0.47f, screen_height * 0.05f}, std::to_string((int)player_score) },
			{ player_pos * scale + adj_pos, std::to_string((int)player_altitude) + "m" }
		};

		for (auto& line : ui)
		{
			if (line != ui.back())
				DrawString(line.first, line.second, olc::GREY);
			else
				if(!player_dead)
					DrawRotatedStringDecal(
						line.first,
						line.second,
						player_angle,
						{ -10.0f, 10.0f },
						olc::GREY,
						olc::vf2d( 1.0f, 1.0f ) * scale);
		}

		// Distinguish Between ESC-pause And Landed-pause
		if (pause && (int)player_altitude) 
		{
			DrawString({ int(screen_width * 0.33f), int(screen_height * 0.7f) }, "         PAUSED\n\nPress SPACE to continue!");
			DrawString({ int(screen_width * 0.05f), int(screen_height * 0.1f) }, "Velocity", olc::DARK_GREY);
			DrawString({ int(screen_width * 0.85f), int(screen_height * 0.1f) }, "Fuel\nLandings", olc::DARK_GREY);
			DrawString({ int(screen_width * 0.45f), int(screen_height * 0.1f) }, "Score", olc::DARK_GREY);
			DrawString({ int(screen_width * 0.55f), int(screen_height * 0.4f), }, "Altitude", olc::DARK_GREY);
		}
	}

	void comms(float fElapsedTime)
	{
		const std::string command[5] = {
			"Eagle, status?", // random questions 0 - 4
			"This is command, status?",
			"How's it going up there?",
			"Eagle, sitrep.",
			"Status report, Eagle.",
		};

		const std::string crew[14] = {
			"This is Eagle, all good.", // responses 0-3
			"We're doing good out here!",
			"We could use some beer,\notherwise good!",
			"Command, we're just fine.",
			"Damn, nice view out here!", // random 4-6
			"I think I can see my house...",
			"Earth is beautiful...",
			"Carefully...", // low alt 7-9
			"Slowly...",
			"Steady...",
			"Capt. James: ", // names 10-12
			"John: ",
			"Fred: ",
			"\nWe're low on fuel!" // fuel
		};

		static bool set = false;
		static bool messageonscreen = false;
		static float randomtime = rand_float(50.0f, 70.0f);
		static float randomtime2 = randomtime / 2;
		static float Time = 0.0f;
		static int randomquestion;
		static int randomresponse;
		static int randomchatter;
		static int randomlowalt;
		static int randomname;
		
		if (!set || player_dead)
		{
			randomquestion = rand() % 5;
			randomresponse = rand() % 4;
			randomchatter = rand() % 3 + 4;
			randomlowalt = rand() % 3 + 7;
			randomname = rand() % 3 + 10;
			set = true;
		}

		// Clock If Not Paused
		if (!pause)
			Time += fElapsedTime;

		// Allow Messsages in ESC-pause
		if (player_altitude > 0.7f)
		{
			if (Time > randomtime)
			{
				// Command Messages
				if (Time < randomtime + 5.0f)
				{
					DrawStringDecal(
						{ screen_width * 0.05f, screen_height * 0.25f },
						"Command:\n" + command[randomquestion],
						olc::GREY);
				}

				// Crew Response
				if (Time > randomtime + 3.0f && Time < randomtime + 8.0f)
				{
					DrawStringDecal(
						{ screen_width * 0.05f, screen_height * 0.3f },
						"Eagle:\n" + crew[randomresponse],
						olc::GREY);
				}

				// Reset Timers When Done
				if (Time > randomtime + 8.0f)
				{
					set = false;
					randomtime = rand_float(50.0f, 70.0f);
					Time = 0.0f;
				}
			}
			static float drawtime = 0.0f;
			static bool timeset = false;

			// Crew Chatter
			if (Time > randomtime / 2 && Time < randomtime / 2 + 5.0f)
			{
				DrawStringDecal(
					{ screen_width * 0.05f, screen_height * 0.35f },
					crew[randomname] + crew[randomchatter], 
					olc::GREY);
			}

			// Low Alt
			if (player_altitude < 4.0f && abs(current_seg_angle) <= 0.349f)
			{
				DrawStringDecal(
					{ screen_width * 0.05f, screen_height * 0.4f },
					crew[randomname] + crew[randomlowalt],
					olc::GREY);
			}
			else
				randomlowalt = rand() % 3 + 7;

			// Fuel
			if (player_fuel < 500)
			{
				DrawStringDecal(
					{ screen_width * 0.05f, screen_height * 0.45f },
					crew[10] + crew[13],
					olc::GREY);
			}
		}
	}

	void title_screen(float fElapsedTime)
	{
		// TODO: CONVERT TO VECTOR OF PAIRS LIKE draw()
		DrawRotatedStringDecal(
			{ screen_width * 0.05f, screen_height * 0.05f },
			"   CONTROLS",
			0.0f, 
			{ 0.0f, 0.0f }, 
			olc::GREY);

		DrawRotatedStringDecal(
			{ screen_width * 0.05f, screen_height * 0.07f },
			"WASD  - movement", 
			0.0f, 
			{ 0.0f, 0.0f }, 
			olc::GREY);

		DrawRotatedStringDecal(
			{ screen_width * 0.05f, screen_height * 0.09f },
			"SHIFT - zoom", 
			0.0f, 
			{ 0.0f, 0.0f }, 
			olc::GREY);

		DrawRotatedStringDecal(
			{ screen_width * 0.05f, screen_height * 0.11f },
			"ESC   - pause",
			0.0f, 
			{ 0.0f, 0.0f },
			olc::GREY);

		DrawString({ int(screen_width * 0.25f), int(screen_height * 0.3f) },
			"LUNAR LANDER", 
			olc::WHITE,
			3);

		DrawString(
			{ int(screen_width * 0.35f), int(screen_height * 0.8f) },
			"Press SPACE to start!", 
			olc::GREY);

		DrawRotatedDecal(
			{ screen_width * 0.52f, screen_height * 0.55f },
			decPlayer.get(), 
			0.4f, 
			{ 8.0f, 8.0f }, 
			{ 7.0f, 7.0f });

		DrawSprite(
			{ int(screen_width * 0.8f), int(screen_height * 0.1f) }, 
			sprEarth.get());

		if (GetKey(olc::SPACE).bPressed) title = false;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::ESCAPE).bPressed)
			pause = true;

		if (GetKey(olc::SPACE).bPressed)
			pause = false;

		if (title)
		{
			title_screen(fElapsedTime);
		}
		else
		{
			draw(fElapsedTime);
			terrain();
			comms(fElapsedTime);
			physics(fElapsedTime);
			//debug();
		}

		return true;
	}
};

int main() {

	lunarLander game;
		if (game.Construct(550, 400, 2, 2/*300, 240, 3, 3*/))
			game.Start();

	return 0;
}