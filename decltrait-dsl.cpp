#include <print>

struct Drawable {
  void print() = delete;
  void resize(int) = delete;
};
struct Rectangle {
  void print() { std::println("Rectangle."); }
};
struct Circle {
  void print() { std::println("Circle."); }
};

namespace hack {

struct DrawableDynTrait {
  virtual void print() = 0;

 protected:
  void* target;
};

struct Drawable_Rectangle : DrawableDynTrait {
  explicit Drawable_Rectangle(Rectangle* tgt) { target = tgt; }
  void print() override { static_cast<Rectangle*>(target)->print(); }
};

struct Drawable_Circle : DrawableDynTrait {
  explicit Drawable_Circle(Circle* tgt) { target = tgt; }
  void print() override { static_cast<Circle*>(target)->print(); }
};

template <class DynTrait>
class TraitPtr {
  friend struct HackFactory;

 public:
  TraitPtr() : storage_{0} {}
  TraitPtr(const TraitPtr&) = default;
  TraitPtr(TraitPtr&&) = default;
  TraitPtr& operator=(const TraitPtr&) = default;
  TraitPtr& operator=(TraitPtr&&) = default;

  DynTrait* operator->() { return reinterpret_cast<DynTrait*>(storage_); }
  bool has_value() const {
    return reinterpret_cast<const void*>(storage_) != nullptr;
  }
  operator bool() const { return has_value(); }
  friend bool operator==(const TraitPtr& self, std::nullptr_t) { return !self; }

 private:
  alignas(DynTrait) char storage_[sizeof(void*) * 2];
};

struct HackFactory {
  static auto DeclTrait_Drawable(Rectangle* target) {
    TraitPtr<DrawableDynTrait> ptr;
    new (ptr.storage_) Drawable_Rectangle{target};
    return ptr;
  }

  static auto DeclTrait_Drawable(Circle* target) {
    TraitPtr<DrawableDynTrait> ptr;
    new (ptr.storage_) Drawable_Circle{target};  // UB, but let's hack it
    return ptr;
  }
};

}  // namespace hack

int main() {
  Rectangle rect;
  Circle circle;
  auto trait = hack::HackFactory::DeclTrait_Drawable(&rect);
  trait->print();  // prints Rectangle.
  trait = hack::HackFactory::DeclTrait_Drawable(&circle);
  trait->print();  // prints Circle.
}
