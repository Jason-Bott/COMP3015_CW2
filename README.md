# COMP3015_CW2

## Introduction

This is 'To The Bridge' my game for COMP3015 CW2. The aim of the game is for the player to navigate a series of corridors that are near identical. Some corridors however have slight or major variations to them. It is up to the player to decide whether the corridor they are in is the normal they are expecting or has been changed in some way. The only way to reach the end is to correctly identify the corridors until the player reaches 'The Bridge'.

## Repository Setup

The folder titled Coursework contains the source code and visual studio solution for the scene.
The folder titled To The Bridge contains the executable with all necessary assets/dependencies. (Download this to play the game)

## Running The Executable

To run the game you will first need to download the folder titled 'To The Bridge' and extract it if necessary.
In the folder there will be a file named 'To The Bridge.exe', opening this will run the game.
All assets and dependencies needed should be in the folder with the executable.

## Video Link

## Game Mechanics

### Player Movement

- This feature allows the player to move and look around the corridor.
	- WASD moves the player forwards, backwards, left and right.
	- Mouse movements turn the players camera in the respective direction.

### Collisions

- This feature prevents the player from walking through the walls and leaving the confines of the corridor.
	- If the player is attempting to move to a position beyond the wall they have their position limited to the nearest point inside.
	- The same applies for doorframes, if attempting to move into it the players position will be set back to a valid position.

### Spaceships

- This feature adds spaceships outside of the corridor that fly around.
	- The spaceships have a set path and order in which they fly past the corridor.
	- They are given a start point and end point and told to travel from A to B at a set speed.

### Timer

- This feature adds a timer hidden during gameplay but used to show a time taken at the end.
	- The timer starts when the player hits space to start the game.
	- The timer stops when the player leaves the final corridor correctly.
	- This is then displayed to the player in the final winning screen.

### Corridor Variants

- This feature adds the ability for corridors to have a slight change to them.
	- After the first corridor, which is always normal, a random variant is selected.
	- There is a 50% chance the corridor is Normal.
	- There is a 30% chance the corridor has a Basic variation (5 in total).
		- A variation that may not be noticed immediately but after careful examination can be spotted.
	- There is a 10% chance the corridor has an Obvious variation (3 in total).
		- A variation that most likely will be spotted immediately or soon after.
	- There is a 10% chance the corridor has an Obscure variation (2 in total).
		- A variation that will be missed most of the time unless examined really closely.

### Corridor Numbers

- This feature tells the player whether they are getting closer to the bridge or not.
	- The number of the corridor will be shown above the middle window.
	- The player starts at 8 and needs to lower the number to 0 (The Bridge).
	- Moving forwards when the corridor is normal will lower the number.
	- Heading back the way you came if there is a variation will lower the number.
	- Taking the wrong path will reset the number back to 8.

## Shader Features

### 1 - Shadows

For the shadows I opted for using shadow maps. This is located in my main shader and contains two render passes.
The first pass takes the POV of the light creating the shadow map based on what it can see.
The second pass then renders the scene normally altering each fragment based on the map produced creating the shadow effect. 

### 2 - Fire Particle Effects

The particle effects are in their own shader, the particle shader. I have used transformative feedback to keep the effect continuously running.
Each particle generates at a random position along the bottom with an initial velocity pushing it upwards.
As the particle ages its colour also weakens until it is completely transparent and is reset.
This creates the looping fire effect.

### 3 - PBR

PBR is located in the main shader along with the shadows. It can be seen in the shadows second pass when it renders the scene not when it generates a shadow map. PBR is used to find the overall colour of a fragment and then the shadow multiplier calculated using the shadow map is applied to this.

### Others

Most of these features were in my CW1 project however they may have been altered slightly while adding the features above.

#### Menus

This feature was not in my CW1 project and is brand new however not one of the marked features like above.
The menus have their own shader which very simply takes a premade image of a menu and binds it to a quad that fills the screen.
As this is covering the whole screen, any time this shader is in use the other shaders will not be.

#### Skybox (CW1)

The skybox feature has been separated from everything else and is now located in its own set of shaders.
This was done as I wanted to keep each feature in its own shader as much as possible and the skybox was a simple one to start with.

#### Multiple Textures and Multilight (CW1)

Multiple textures has been kept roughly the same and is still used in the main shader along with PBR.
Multilight however has had to be changed to accompany PBR. The logic behind using multiple lights has remained the same. Only the values the lights themselves contain has been altered.

## Sample Screens

### Instructions
![Instructions](/SampleScreens/Instructions.png)

### Example of a Normal Corridor

![Normal](/SampleScreens/Normal.png)

### Example of a Variant Corridor

![Variant](/SampleScreens/Variant.png)

## Evaluation

### Achievements

I am extremely happy with what I have created. 
Despite difficulties I managed to create the experience I was aiming for.
I have managed to add in the fire particle effects and shadows as I had planned.
Beyond this I also added in the full gameplay loop I wanted to. This includes 10 total variations which I am really happy with.
Overall I am pleased with the scene I have made and the features implemented.

### What I Would Do Differently

Although I am very pleased with what I have made there are some improvements that I would like to make.
Firstly I would like to improve upon the lighting. I am happy with the interior lighting of the alarm. However, I feel the light from outside could be improved on. This is something I believe will make the game even better. As well as adding more variations to make the game even more replayable.

## Credits

### Models

Spaceship model by Ebal Studios on Sketchfab: https://sketchfab.com/EbalStudios
All other models created by myself in blender.

### Skybox

Skybox by Westbeam on OpenGameArt: https://opengameart.org/content/space-skyboxes-1

### Sounds

Sound from freesound_community on Pixabay: https://pixabay.com/users/freesound_community-46691455/
