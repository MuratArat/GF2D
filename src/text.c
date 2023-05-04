#include "text.h"
#include "simple_logger.h"

int Text_Init() {
    if (TTF_Init() == -1) {
        slog("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return -1;//error
    }
    return 0;
}

void Text_Quit() {
    TTF_Quit();//quit SDL_ttf
}

Text *Text_Load(TTF_Font *font, const char *text, SDL_Color color, SDL_Renderer *renderer) {
    Text *result = (Text *)malloc(sizeof(Text));
    if (result == NULL) {
        slog("Unable to allocate memory for Text structure\n");
        return NULL;//error
    }

    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    if (textSurface == NULL) {
        slog("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        free(result);
        return NULL;//error
    }

    result->texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (result->texture == NULL) {
        slog("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        free(result);
        return NULL;//error
    }

    result->width = textSurface->w; //get width and height
    result->height = textSurface->h; //get width and height

    SDL_FreeSurface(textSurface); //free surface

    return result; //success
}

void Text_Free(Text *text) {
    if (text == NULL) {
        return;//error
    }

    SDL_DestroyTexture(text->texture);
    free(text);//free text
}

void Text_Render(Text *text, SDL_Renderer *renderer, int x, int y) {
    if (text == NULL || renderer == NULL) {
        return;//error
    }

    SDL_Rect renderQuad = {x, y, text->width, text->height}; //set rendering space and render to screen
    SDL_RenderCopy(renderer, text->texture, NULL, &renderQuad); //render to screen
}