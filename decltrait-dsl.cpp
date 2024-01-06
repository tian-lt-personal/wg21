#include <cassert>
#include <print>

struct Drawable {
  void print();
  void resize(int);
};
struct Rectangle {
  void print() { std::println("Rectangle."); }
};
struct Circle {
  void print() { std::println("Circle."); }
};

namespace hack {

struct DrawableTrait {
  virtual void print() { throw; }

 protected:
  void* target;
};

struct Drawable_Rectangle : DrawableTrait {
  explicit Drawable_Rectangle(Rectangle* tgt) { target = tgt; }
  void print() override { static_cast<Rectangle*>(target)->print(); }
};

struct Drawable_Circle : DrawableTrait {
  explicit Drawable_Circle(Circle* tgt) { target = tgt; }
  void print() override { static_cast<Circle*>(target)->print(); }
};

class DrawableTraitPtr {
  friend DrawableTraitPtr DeclTrait_Drawable(Rectangle* target);
  friend DrawableTraitPtr DeclTrait_Drawable(Circle* target);

 public:
  DrawableTraitPtr() : storage_{0} {}
  DrawableTraitPtr(const DrawableTraitPtr&) = default;
  DrawableTraitPtr(DrawableTraitPtr&&) = default;
  DrawableTraitPtr& operator=(const DrawableTraitPtr&) = default;
  DrawableTraitPtr& operator=(DrawableTraitPtr&&) = default;

  DrawableTrait* operator->() {
    return reinterpret_cast<DrawableTrait*>(storage_);
  }
  bool has_value() const {
    return reinterpret_cast<const void*>(storage_) != nullptr;
  }
  operator bool() const { return has_value(); }

 private:
  alignas(DrawableTrait) char storage_[sizeof(void*) * 2];
};

DrawableTraitPtr DeclTrait_Drawable(Rectangle* target) {
  DrawableTraitPtr ptr;
  new (ptr.storage_) Drawable_Rectangle{target};
  return ptr;
}

DrawableTraitPtr DeclTrait_Drawable(Circle* target) {
  DrawableTraitPtr ptr;
  new (ptr.storage_) Drawable_Circle{target};  // UB, but let's hack it
  return ptr;
}

}  // namespace hack

int main() {
  Rectangle rect;
  Circle circle;
  auto trait = hack::DeclTrait_Drawable(&rect);
  assert(trait);
  trait->print();  // prints Rectangle.
  trait = hack::DeclTrait_Drawable(&circle);
  assert(trait);
  trait->print();  // prints Circle.
}
