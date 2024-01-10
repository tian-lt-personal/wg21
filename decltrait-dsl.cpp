#include <print>

struct Drawable {
  void print() = delete;
  void resize(int) = delete;
};
struct Rectangle {
  Rectangle() = default;
  ~Rectangle() { std::println("destruct Rectangle."); }
  void print() { std::println("Rectangle."); }
};
struct Circle {
  Circle() = default;
  ~Circle() { std::println("destruct Circle."); }
  void print() { std::println("Circle."); }
};

namespace hack {
struct Factory;
template <class>
class TraitPtr;

namespace details {

struct TypeErased {};

struct DrawableVTable {
  using dtor_t = void (*)(void*);
  using sig_void_void_t = void (TypeErased::*)();
  dtor_t __dtor;
  sig_void_void_t print;
};

class DrawableTrait {
  friend struct hack::Factory;

  template <class>
  friend class hack::TraitPtr;

 public:
  void print() {
    auto t = static_cast<TypeErased*>(target_);
    (t->*(vptr_->print))();
  }

 private:
  explicit DrawableTrait(DrawableVTable* vptr, void* target) noexcept
      : vptr_(vptr), target_(target) {}
  DrawableVTable* vptr_;
  void* target_;
};

}  // namespace details

template <class Trait>
class TraitPtr {
  friend struct Factory;

 public:
  void destroy() { trait_.vptr_->__dtor(trait_.target_); }
  Trait* operator->() { return &trait_; }
  bool has_value() const { return trait_.target_ != nullptr; }

 private:
  explicit TraitPtr(Trait trait) : trait_(trait) {}
  Trait trait_;
};

struct Factory {
  template <class T>
  static auto decltrait_Drawable(T* target) {
    static details::DrawableVTable vtable{
        .__dtor = [](void* t) { static_cast<T*>(t)->~T(); },
        .print = reinterpret_cast<details::DrawableVTable::sig_void_void_t>(
            &T::print)};
    return TraitPtr<details::DrawableTrait>{
        details::DrawableTrait{&vtable, target}};
  }
};
}  // namespace hack

int main() {
  Rectangle rect;
  Circle circle;
  auto trait = hack::Factory::decltrait_Drawable(&rect);
  trait->print();   // prints Rectangle.
  trait.destroy();  // prints destruct Rectangle.
  trait = hack::Factory::decltrait_Drawable(&circle);
  trait->print();   // prints Circle.
  trait.destroy();  // prints destruct Circle.
}
