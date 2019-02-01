#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

// Structure pour contenir des Point/Vecteur 2 dimensions.
struct Vec2i{
    int x;
    int y;
    
    int length(){
        return trunc(sqrt(x * x + y * y));
    }
};

// Fonction qui retourne la distance entre 2 points.
int distance(int firstPointX, int firstPointY, int secondPointX, int secondPointY){
    return trunc(sqrt((firstPointX - secondPointX) * (firstPointX - secondPointX) +
                (firstPointY - secondPointY) * (firstPointY - secondPointY)));
}

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    bool boostUsed = false;
    bool firstFrame = true;
    
    Vec2i speed;
    Vec2i opponentSpeed;

    Vec2i lastPosition;
    Vec2i lastOpponentPosition;
    
    // game loop
    while (1) {
        int x;
        int y;
        int nextCheckpointX; // x position of the next check point
        int nextCheckpointY; // y position of the next check point
        int nextCheckpointDist; // distance to the next checkpoint
        int nextCheckpointAngle; // angle between your pod orientation and the direction of the next checkpoint
        cin >> x >> y >> nextCheckpointX >> nextCheckpointY >> nextCheckpointDist >> nextCheckpointAngle; cin.ignore();
        int opponentX;
        int opponentY;
        cin >> opponentX >> opponentY; cin.ignore();
        
        if(firstFrame){
            lastPosition.x = x;
            lastPosition.y = y;
            firstFrame = false;
        }
        
        // Calcul des vitesses
        speed.x = x - lastPosition.x;
        speed.y = y - lastPosition.y;
        opponentSpeed.x = opponentX - lastOpponentPosition.x;
        opponentSpeed.y = opponentY - lastOpponentPosition.y;
        
        cerr << "Pod Speed : (" << speed.x << ", " << speed.y << ") : " << speed.length() << endl;
        cerr << "Opponent Speed : (" << opponentSpeed.x << ", " << opponentSpeed.y << ") : " << opponentSpeed.length() << endl;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // You have to output the target position
        // followed by the power (0 <= thrust <= 100)
        // i.e.: "x y thrust"
        int thrust = 0;
        float directionCoef = 0.f;
        bool boost = false;
        
        if(nextCheckpointAngle > 90 || nextCheckpointAngle < -90)
            thrust = 0;
        // Si le checkpoint est quasi tout droit, on boost.
        else if(nextCheckpointAngle < 5 && nextCheckpointAngle > -5 && distance(x, y, nextCheckpointX, nextCheckpointY) > 2000 && boostUsed == false){
            boost = true;
        }
        else{
            // Si le checkpoint est entre "effectiveAngle" et 90°, alors on ralenti.
            int effectiveAngle = 65;

            directionCoef = std::min(((float)(90 - abs(nextCheckpointAngle))/(90 - effectiveAngle)), 1.f);
            // directionCoef = std::max(directionCoef, 0.f);
            thrust = directionCoef * 100;

            cerr << "Direction coefficient : " << directionCoef << endl;
            cerr << "Puissance : " << thrust << endl;
        }
            
        // Si le vaisseau adverse est proche, et que notre vaisseau à suffisamment de vitesse, on va chercher
        // à pousser le vaisseau ennemi de sa trajectoire. 
        if(distance(x, y, opponentX, opponentY) < 1000 && speed.length() > 500){
            cout << opponentX << " " << opponentY << " " << 100 << endl;
            continue;
        }
        else
            cout << nextCheckpointX << " " << nextCheckpointY << " ";
            
        if(boost){
            cout << "BOOST";
            boostUsed = true;
        }
        else
            cout << thrust;
        
        cout << endl;
        
        lastPosition.x = x;
        lastPosition.y = y;
        
        lastOpponentPosition.x = opponentX;
        lastOpponentPosition.y = opponentY;
    }
}
