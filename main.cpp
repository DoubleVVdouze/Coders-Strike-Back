#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

// Structure pour contenir des Point/Vecteur 2 dimensions.
struct Vec2i{
    int x;
    int y;
    
    // Constructeurs
    Vec2i(){
        x = 0;
        y = 0;
    }
    
    Vec2i(int x1, int y1){
        x = x1;
        y = y1;
    }
    
    Vec2i operator + ( const Vec2i& v ) const
    {
        return Vec2i(x + v.x, y + v.y);
    }
    
    // Retourne la longueur du vecteur.
    int length(){
        return trunc(sqrt(x * x + y * y));
    }
     
    // retourne l'angle entre deux vecteur. De -180° a 180°.   
    int angle(Vec2i& v){
        int dot = x*v.x + y*v.y;
        int det = x*v.y - y*v.x;
        return trunc(std::atan2(det, dot)*180/3.14);
    }
};

// Fonction qui retourne la distance entre 2 points.
int distance(int firstPointX, int firstPointY, int secondPointX, int secondPointY){
    return trunc(sqrt((firstPointX - secondPointX) * (firstPointX - secondPointX) +
                (firstPointY - secondPointY) * (firstPointY - secondPointY)));
}

// Fonction qui retourne vrai si il y a une collision à la prochaine frame
// TODO : à améliorer, pas très précise.
bool predictCollision(Vec2i positionPod, Vec2i speedPod, Vec2i positionOpponentPod, Vec2i speedOpponentPod){
    Vec2i nextPositionPod = positionPod + speedPod;
    Vec2i nextPositionOpponentPod = positionOpponentPod + speedOpponentPod;
    
    if(distance(nextPositionPod.x, nextPositionPod.y, nextPositionOpponentPod.x, nextPositionOpponentPod.y) < 700)
        return true;
    
    return false;
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

    Vec2i position;
    Vec2i opponentPosition;
    
    Vec2i speed;
    Vec2i opponentSpeed;

    Vec2i lastPosition;
    Vec2i lastOpponentPosition;
    
    int lastCheckpointAngle;
    
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

        position = Vec2i(x, y);
        opponentPosition = Vec2i(opponentX, opponentY);

        int distanceFromNextCheckpoint = distance(x, y, nextCheckpointX, nextCheckpointY);
        int oppenentDistanceFromNextCheckpoint = distance(opponentX, opponentY, nextCheckpointX, nextCheckpointY);
        int distanceFromOpponent = distance(x, y, opponentX, opponentY);
        
        int thrust = 0;
        float directionCoef = 0.f;
        float slowingFromDistance = 0.f;
        
        bool boost = false;
        bool shield = false;
        
        Vec2i target;
        
        if(firstFrame){
            lastPosition.x = x;
            lastPosition.y = y;
            lastOpponentPosition.x = opponentX;
            lastOpponentPosition.y = opponentY;
            
            lastCheckpointAngle = nextCheckpointAngle;
            
            firstFrame = false;
        }   
        
        // Calcul des vitesses
        speed.x = x - lastPosition.x;
        speed.y = y - lastPosition.y;
        opponentSpeed.x = opponentX - lastOpponentPosition.x;
        opponentSpeed.y = opponentY - lastOpponentPosition.y;
        
        cerr << "Pod Speed : (" << speed.x << ", " << speed.y << ") : " << speed.length() << endl;
        cerr << "Opponent Speed : (" << opponentSpeed.x << ", " << opponentSpeed.y << ") : " << opponentSpeed.length() << endl;

        cerr << "Angle : " << speed.angle(opponentSpeed) << endl;
        cerr << "checkpointAngle : " << nextCheckpointAngle << endl;
        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // You have to output the target position
        // followed by the power (0 <= thrust <= 100)
        // i.e.: "x y thrust"
        
        if(nextCheckpointAngle > 90 || nextCheckpointAngle < -90)
            thrust = 10;
        // Si le checkpoint est quasi tout droit, on boost.
        else if(nextCheckpointAngle < 2 && nextCheckpointAngle > -2 && distanceFromNextCheckpoint > 6000 && boostUsed == false){
            boost = true;
        }
        else{

            // Si on arrive proche d'un checkpoint, on ralenti selon l'angle par lequelle on arrive.
            // note : ici les chiffres sont totalement arbitraire
            if(distanceFromNextCheckpoint < 3000){
                if(nextCheckpointAngle < 10 && nextCheckpointAngle > -10)
                    slowingFromDistance = abs(nextCheckpointAngle * 7.5);
                else
                    slowingFromDistance = 10;
            }
            else
                slowingFromDistance = 0.f;


            // Si le checkpoint est entre "effectiveAngle" et 90°, alors on ralenti.
            int effectiveAngle = 55;

            directionCoef = min(((float)(90 - abs(nextCheckpointAngle))/(90 - effectiveAngle)), 1.f);

            thrust = directionCoef * (100 - slowingFromDistance);

            cerr << "Direction coefficient : " << directionCoef << endl;
            cerr << "Distance coefficient : " << slowingFromDistance << endl;
            cerr << "Puissance : " << thrust << endl;
            cerr << "Distance From Checkpoint : " << (float)distanceFromNextCheckpoint << endl;
        }
        
        // Si un des deux Pod va assez vite, que l'angle des vecteurs vitesses est superieur ou égal à 15 et que à la frame suivante il va y avoir une collision, alors on shield.
        if((opponentSpeed.length() > 350 ||  speed.length() > 350) && abs(speed.angle(opponentSpeed)) >= 15 && predictCollision(position, speed, opponentPosition, opponentSpeed)){
            shield = true;
        }
            
        // Si le vaisseau adverse est proche, et que notre vaisseau à suffisamment de vitesse, on va chercher
        // à pousser le vaisseau ennemi de sa trajectoire. 
        if(distanceFromOpponent < 1000 && speed.length() > 500 && distanceFromNextCheckpoint > oppenentDistanceFromNextCheckpoint){
            target.x = opponentX + opponentSpeed.x;
            target.y = opponentY + opponentSpeed.y;
            thrust = 100;
        }
        else {
            target.x = nextCheckpointX;
            target.y = nextCheckpointY;
        }
        
        cout << target.x << " " << target.y << " ";
        
        if(boost){
            cout << "BOOST";
            boostUsed = true;
        }
        else if(shield){
            cout << "SHIELD";
        }
        else
            cout << thrust;
        
        cout << endl;
        
        // Pour calculer les différences avec la prochaine frame.
        lastPosition.x = x;
        lastPosition.y = y;
        
        lastOpponentPosition.x = opponentX;
        lastOpponentPosition.y = opponentY;
        
        lastCheckpointAngle = nextCheckpointAngle;
    }
}
