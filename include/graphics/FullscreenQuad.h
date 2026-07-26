//
// Created by hhabib on 27/07/2026.
//

class FullscreenQuad
{
public:

  FullscreenQuad();
  ~FullscreenQuad();
  void initialize();
  void draw() const;

private:

  unsigned int n_vao = 0;
  unsigned int n_vbo = 0;
};