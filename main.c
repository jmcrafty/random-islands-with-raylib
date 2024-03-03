/*******************************************************************************************
*
*   random islands with raylib
*
*   Psuedo-random generation of landmass using perlin noise
*
*   Code based on Playing with Perlin Noise: Generating Realistic Archipelagos by Yvan Scher on Medium.com
*   https://medium.com/@yvanscher/playing-with-perlin-noise-generating-realistic-archipelagos-b59f004d8401
* 
*   Example originally created with raylib 5.0
*
********************************************************************************************/

#include "raylib.h"
#include "math.h"

#define RAYGUI_IMPLEMENTATION
#ifdef RAYGUI_IMPLEMENTATION
#include "raygui.h" // Must be manually added to C:/raylib/raylib/src
#endif

enum texture_display {
    PERLIN_NOISE,
    COMBINED_GRADIENT,
    LAND_RENDER,
    ISLAND_RENDER
};

// Globally defined textures which are used in main loop and function
Texture2D perlinTexture;
Texture2D roundedTexture;
Texture2D landTexture;
Texture2D islandTexture;

// Colors for each layer of the landmass
Color lightblue =   {88, 153, 237, 255};
Color blue =        {65, 105, 225, 255};
Color green =       {34, 139, 34, 255};
Color darkgreen =   {0, 100, 0, 255};
Color sandy =       {210, 180, 140, 255};
Color beach =       {238, 214, 175, 255};
Color snow =        {255, 250, 250, 255};
Color mountain =    {139, 137, 137, 255};

//------------------------------------------------------------------------------------
// Change these settings for the initial render
//------------------------------------------------------------------------------------
// The size of the initial perlin image in pixels
Vector2 world_shape = {1024, 1024};

// Increase this value to have less total land above sea level
// Normal values: ( 1.0f to 255.0f )
float land_threshold = 100;

// Increase this value to have a wider island area
// Normal values: ( 1.0 to 10.0 )
float land_radius = 10.0f;

// Change this value to use a different area of the perlin image for rendering
// Normal values: ( 1.0f to 1000.0f )
float perlin_offset = 0.0f;

// Increase this value to create more island segments
// Normal values: ( 1.0f to 20.0f )
float perlin_scale = 10.0f;


//------------------------------------------------------------------------------------
// Call this function to redraw the island using current settings
//------------------------------------------------------------------------------------
void generate_island(void) {
    
    // Generate a perlin noise gradient (must be called after InitWindow)
    Image perlinImage = GenImagePerlinNoise((int)world_shape.x, (int)world_shape.y, perlin_offset, perlin_offset, perlin_scale);

    // Create a texture (saved in GPU instead of CPU)
    perlinTexture = LoadTextureFromImage(perlinImage);

    // Create an array of image colors
    Color *perlinColors = LoadImageColors(perlinImage);

    // Overlay a gradient circle on the perlin noise to create a rounded island system
    Image roundedImage = GenImageColor(world_shape.x, world_shape.y, WHITE);
    int center_x = (int)(world_shape.x / 2.0f);
    int center_y = (int)(world_shape.x / 2.0f);

    // Sets the maximum width of the island draw area
    float max_width = sqrt(center_x*center_x + center_y*center_y) * (land_radius / 10.0f );

    for (int i = 0; i < (int)world_shape.x; i++) {
        for (int j = 0; j < (int)world_shape.y; j++) {

            // Get 1D array index using 2D coordinates
            int index = (j * (int)world_shape.x) + i;

            // Compute the circular distance scalar
            float distx = abs(i - center_x);
            float disty = abs(j - center_y);
            float dist = sqrt(distx*distx + disty*disty) / max_width;
            dist = -((dist - 0.5f) * 2.0f);

            // Scale the existing color
            int color_index = perlinColors[index].r * dist;
            if (color_index<0) color_index=0;
            Color combine_grad = {color_index, color_index, color_index, 255};
            ImageDrawPixel(&roundedImage, i, j, combine_grad);
        }
    }

    // Create a texture (saved in GPU instead of CPU)
    roundedTexture = LoadTextureFromImage(roundedImage);

    // LoadImageColors creates a 1D array of color data from an image
    Color *roundedColors = LoadImageColors(roundedImage);

    // Loop over the array and assign color to height values
    Image landImage = GenImageColor(world_shape.x, world_shape.y, WHITE);
    Image islandImage = GenImageColor(world_shape.x, world_shape.y, blue);
    for (int i = 0; i < (int)world_shape.x; i++) {
        for (int j = 0; j < (int)world_shape.y; j++) {
            
            // Get 1D array index using 2D coordinates
            int index = (j * (int)world_shape.x) + i;

            // Add pixels above the land_threshold to our landmass
            if (roundedColors[index].r < land_threshold )
                ImageDrawPixel(&landImage, i, j, BLACK);

            // For each pixel set a color based on the height land_threshold
            if (roundedColors[index].r < land_threshold )
                ImageDrawPixel(&islandImage, i, j, blue);
            else if (roundedColors[index].r < land_threshold + 10 )
                ImageDrawPixel(&islandImage, i, j, lightblue);
            else if (roundedColors[index].r < land_threshold + 12 )
                ImageDrawPixel(&islandImage, i, j, beach);
            else if (roundedColors[index].r < land_threshold + 14 )
                ImageDrawPixel(&islandImage, i, j, sandy);
            else if (roundedColors[index].r < land_threshold + 25 )
                ImageDrawPixel(&islandImage, i, j, green);
            else if (roundedColors[index].r < land_threshold + 55 )
                ImageDrawPixel(&islandImage, i, j, darkgreen);
            else if (roundedColors[index].r < land_threshold + 80 )
                ImageDrawPixel(&islandImage, i, j, mountain);
            else
                ImageDrawPixel(&islandImage, i, j, snow);
        }
    }

    // Create a texture (saved in GPU instead of CPU)
    landTexture = LoadTextureFromImage(landImage);

    // Create a texture (saved in GPU instead of CPU)
    islandTexture = LoadTextureFromImage(islandImage);

    // Once the texture is generated we can unload CPU resources
    UnloadImage(perlinImage);
    UnloadImage(roundedImage);
    UnloadImage(landImage);
    UnloadImage(islandImage);
    UnloadImageColors(perlinColors);
    UnloadImageColors(roundedColors);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "random islands with raylib");

    // Set the initial island
    generate_island();

    // Set the current texture to render
    int current_display = ISLAND_RENDER;

    // Offset the position nicely in the center of the screen
    Vector2 tex_pos = {(screenWidth - world_shape.x) / 2.0f,
                       (screenHeight - world_shape.y) / 2.0f};

    // Flag for pending generate request
    #ifdef RAYGUI_IMPLEMENTATION
    bool button_pressed = false;
    #endif

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Cycle through images
        if (IsKeyPressed(KEY_A)) {
            current_display--;
            if (current_display < 0) {
                current_display = ISLAND_RENDER;
            }
        }
        if (IsKeyPressed(KEY_S)) {
            current_display++;
            if (current_display > ISLAND_RENDER) {
                current_display = 0;
            }
        }
        // Redraw island with GUI settings
        #ifdef RAYGUI_IMPLEMENTATION
        if (button_pressed) {
            button_pressed = false;
            generate_island();
        }
        #endif

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            // Allow transitions between textures
            switch (current_display) {
                case PERLIN_NOISE :
                    ClearBackground(BLACK);
                    DrawTexture(perlinTexture, tex_pos.x, tex_pos.y, WHITE);
                    break;
                case COMBINED_GRADIENT :
                    ClearBackground(BLACK);
                    DrawTexture(roundedTexture, tex_pos.x, tex_pos.y, WHITE);
                    break;
                case LAND_RENDER :
                    ClearBackground(blue);
                    DrawTexture(landTexture, tex_pos.x, tex_pos.y, WHITE);
                    break;
                case ISLAND_RENDER :
                    ClearBackground(blue);
                    DrawTexture(islandTexture, tex_pos.x, tex_pos.y, WHITE);
                    break;
            }

            // GUI settings
            #ifdef RAYGUI_IMPLEMENTATION
            DrawRectangle( 5, 5, 210, 155 , (Color){255, 255, 255, 128});
            GuiSliderBar((Rectangle){ 30, 50, 100, 10 }, TextFormat("%3.0f", land_threshold), "land_threshold", &land_threshold, 1.0f, 255.0f);
            GuiSliderBar((Rectangle){ 30, 70, 100, 10 }, TextFormat("%2.0f", land_radius), "land_radius", &land_radius, 1.0f, 10.0f);
            GuiSliderBar((Rectangle){ 30, 90, 100, 10 }, TextFormat("%3.0f", perlin_offset), "perlin_offset", &perlin_offset, 0.0f, 1000.0f);
            GuiSliderBar((Rectangle){ 30, 110, 100, 10 }, TextFormat("%2.0f", perlin_scale), "perlin_scale", &perlin_scale, 1.0f, 20.0f);
            button_pressed = GuiButton((Rectangle){ 10, 130, 100, 20 }, "generate!");
            #endif
            DrawText("random islands: press \"a\" and \"s\" \nto cycle through renders", 10, 10, 10, RAYWHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(perlinTexture);
    UnloadTexture(roundedTexture);
    UnloadTexture(landTexture);
    UnloadTexture(islandTexture);

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}