#include "garden.h"


    //
    // Simulate stone
    //

void updateAndRender() {

  if (!stone.dead) {
    // Legg til akselerasjon til farten
    vec3 tmp_acc;
    copyV3(stone.acc, tmp_acc);
    scaleV3(tmp_acc, global.dt);
    addV3(stone.speed, tmp_acc);
      
    // Legg til summen av fart til posisjon
    vec3 tmp_speed;
    copyV3(stone.speed, tmp_speed); 
    scaleV3(tmp_speed, global.dt);
    addV3(stone.pos, tmp_speed);
      
      
    // Rotér steinen med nåværende spin_speed
    stone.spin += stone.spin_speed * global.dt;
      
      
    // @FORBEDRING - ønsker å legge til friksjon langs horsintalen også... Dette kan skje både i luften (luftmotsatnd) og ved kontakt med vannet her..
      
    if (stone.pos[1] < 0) {
      stone.hops  += 1;
      stone.pos[1] = 0;
        
      // Friksjonen avhenger ved vinkelen steinen treffer vannet ved, i forhold til horisontalen. Denne går mellom 0 og 1.56, og vi vil skalerer disse verdiene til å gå mellom 1 og 0, hvor 1 bevarer all fart og 0 ikke bevarer noe fart. -bl 14.02.2020
        
      // @FORBEDRING! Friksjon regnes ut på en dum måte nå. Vinkelen på kastet er nå samme som vinkelen kameraet ser med horisontalen. Dette er greit. Friksjonen er derimot avhengig av denne vinkelen, som ikke stemmer helt... Vikipedia sier at den optimale vinkelen skal være 20 grader, altså 0.349 radianer. Vi må sette denne til å gi størst bevaring (0.8 / 0.9 eller noe), og la bevaringen gå ned på noe vis med høyere og lavere vinkler.

      // @MATTE noe er feil med utregningen av denne vinkelen... den skal liksom svare til hvor mange grader opp fra xz-planet den er, men det er den ikke... Kan være noe feil i 'angleV3'-funksjonen.... Hurra for bugs!

      // @USIKKER vi snur steinens fart FØR vi sjekker vinkel :D
      stone.speed[1] = -stone.speed[1];
        
      vec3 xz_dir = {stone.speed[0], 0.0f, stone.speed[2]}; // Isoler xz-komponenten
      normalizeV3(xz_dir); // @USIKKER tror man må normalisere??? Vet ikke
      float theta = angleV3(stone.speed, xz_dir); // Finn vinkelen til kastet langs xz-komponenten

      //printf("deg: %f\n", theta * 180 / 3.1415);
      //printf("fri: %f\n", fabs(theta-0.349066f)*4.0f);
      // float theta = -atan(stone.speed[1]/stone.speed[0]) / 1.56f; // Den gamle måten å regne ut vinkel.. Tror denne stemte?

      float friction = (1.0f - min(fabs(0.349-theta)*5.0f, 1.0f))*0.9f;
              
      // Flipp y-retning (siden vi har truffet vannet, og møter "uendelig stor masse".
        
      scaleV3(stone.speed, friction); // Skaler hele farten med friksjonen!!!
      stone.spin_speed *= friction;   // Reduser også spinne-hastigheten
    }
     
    // Når farten er liten nok bare slutter vi å simuler steinen.
    if (lenV3(stone.speed) < 0.5f) {
      stone.dead = 1;
      //printf("Hops: %i\n", stone.hops);
    }
  }
    
    
    
  //
  // Render first stone
  //
    
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
  glUseProgram(main_shader.id);
  glBindVertexArray(stoneVAO);
    
  //setInt(shader_program, "texture0", 0);
  setVec3(main_shader.id, "view_color",   cam.pos);
  setVec3(main_shader.id, "object_color", stone.color);
  setVec3(main_shader.id, "light_color",  light_cube.color);
  setVec3(main_shader.id, "light_pos",    light_cube.pos);    
    
  // Calculate perspective
  perspective(cam.fov, aspect_ratio, 0.1f, 100.0f, proj);
  setMat4(main_shader.id, "proj", proj);

  // Load in the vectors that are used to make the lookAt-matrix
  setVec3(main_shader.id, "cam_pos",   cam.pos);
  setVec3(main_shader.id, "cam_front", cam.front);
  setVec3(main_shader.id, "cam_up",    cam.up);

  // Transler og tegn første kube


  // @TANKER - rekkefølge for skalering
  // DONE - Skaler modell
  // DONE - Roter langs y-akse med tid
  // TODO - Vend bakover mot utkaststed.... må tenke litt på dette, må nok gjøres med en generell rotasjonsmatrise: https://en.wikipedia.org/wiki/Rotation_matrix#Nested_dimensions (ser ut som GPU-arbeid :) )

  clearM4(model);
   
  mat4 scale;
  clearM4(scale);
  initIdM4(scale);
  mset(scale, 0, 0, 0.5f);
  mset(scale, 1, 1, 0.1f);
  mset(scale, 2, 2, 0.5f);

  rotateY(scale, stone.spin, model); // Roter 'scale' med 'stone.spin' radianer om y-aksen, og lagre resultatet i 'model'
  mkTranslation(model, stone.pos);   // Transler 'model' til stone.pos

  setMat4(main_shader.id, "model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);



  //
  // Tegn 'vannet'
  //
    
  //setInt(shader_program, "texture0", 0);
  setVec3(main_shader.id, "view_color",   cam.pos);
  setVec3(main_shader.id, "object_color", water.color);
  setVec3(main_shader.id, "light_color",  light_cube.color);
  setVec3(main_shader.id, "light_pos",    light_cube.pos);    
    
  // Calculate perspective
  perspective(cam.fov, aspect_ratio, 0.1f, 100.0f, proj);
  setMat4(main_shader.id, "proj", proj);

  // Load in the vectors that are used to make the lookAt-matrix
  setVec3(main_shader.id, "cam_pos",   cam.pos);
  setVec3(main_shader.id, "cam_front", cam.front);
  setVec3(main_shader.id, "cam_up",    cam.up);

  clearM4(model);
  initIdM4(model);
  clearM4(scale);
  initIdM4(scale);
  mset(model, 0, 0, 1000.5f);
  mset(model, 1, 1, 0.005f);
  mset(model, 2, 2, 1000.0f);

  mkTranslation(model, water.pos);   // Transler 'model' til stone.pos

  setMat4(main_shader.id, "model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);



    
  //
  // Tegn lys-kube
  //
    
  glUseProgram(light_shader.id);
  glBindVertexArray(lightVAO);

  setVec3(light_shader.id, "light_color",  light_cube.color);
    
    
  // Calculate perspective
  perspective(cam.fov, aspect_ratio, 0.1f, 100.0f, proj);
  setMat4(light_shader.id, "proj", proj);

  // Load in the vectors that are used to make the lookAt-matrix
  setVec3(light_shader.id, "cam_pos",   cam.pos);
  setVec3(light_shader.id, "cam_front", cam.front);
  setVec3(light_shader.id, "cam_up",    cam.up);
  // Transler og tegn andre kube. Denne skaleres også ned med matrisen under.
  clearM4(model);
  initIdM4(model);
  mset(model, 0, 0, 0.5f);
  mset(model, 1, 1, 0.5f);
  mset(model, 2, 2, 0.5f);
        
  mkTranslation(model, light_cube.pos);
    
  setMat4(light_shader.id, "model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glfwSwapBuffers(window);
  glfwPollEvents();
}
