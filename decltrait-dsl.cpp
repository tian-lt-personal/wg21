int SideEffect = 0;

struct Drawable { void print(); };
struct Rectangle { void print() { SideEffect = 1; } };
struct Circle { void print() { SideEffect = 2; } };

struct __UNIQUE_TRAIT_Drawable {
  virtual void print() = 0;
};

struct __UNIQUE_DISPATCH_Drawable_Rectangle
  : __UNIQUE_TRAIT_Drawable {
  void print() override {
    reinterpret_cast<Rectangle*>(this)->print();
  }
};

__UNIQUE_TRAIT_Drawable& __UNIQUE_DECLTRAIT_Drawable(Rectangle* src) {
  return
    *reinterpret_cast<__UNIQUE_DISPATCH_Drawable_Rectangle*>(src); 
}

int main() {
  Rectangle rectangle;
  __UNIQUE_DECLTRAIT_Drawable(&rectangle).print(); // TODO: SIGSEGV.
}
