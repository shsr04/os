#pragma once
#include "kernel/core.hpp"

/// A category C consists of:
///   1. a set of objects Ob(C)
///   2. a set of morphisms Hom(t,u) ⊆ {t->u | t,uϵOb(C)}
///   3. for each object t, an identity morphism id(t) ϵ HomC(t,t)
///   4. a composition formula ∘: (t->u)⨯(u->v) -> (t->v) where t,u,vϵOb(C)
///   where multiple morphisms can be composed associatively
///     and the identity morphism is "neutral"

/// A monoid M[T,op,nil] consists of:
///   1. a singleton set Ob(M):={#}
///   2. a morphism HomM(#,#):={t | tϵT}
///   3. an identity id(#):=nil
///   4. a composition formula ∘:=op
/// Example: the additive monoid of natural numbers: (N,+,0)
template <class T, T N, function<T, T, T> F> class Monoid {
  public:
    const T value_;
    constexpr Monoid(T value) : value_(value) {}
    /// Invoke the monoid's op
    constexpr Monoid<T, N, F> operator()(T p) const { return F(value_, p); }
    /// Get the nil element of type T
    static constexpr T nil = N;
};

/// A group G is a monoid
///   where each object x has an inverse x' such that x∘x'=id(#)
template <class T, T N, function<T, T, T> F, function<T, T> I>
class Group : public Monoid<T, N, F> {
    using Monoid<T, N, F>::id;

  public:
    constexpr Group(T value) : Monoid<T, N, F>(value) {}
    constexpr Monoid<T, N, F> inv() const { return F(I(id())); }
};

/// A functor F: C->C' is a morphism from C to C' that satisfies:
///   1. F(h: t->u) = F(h): F(t)->F(u)
///   2. F(id(t)) = id(F(t))
///   3. F(h'∘h)=F(h')∘F(h)
///   where t,uϵOb(C) and h,h'ϵHomC(t,u)
///   (Note: in the category of categories `Cat`, categories are the objects
///   Ob(Cat) and functors are the morphisms HomCat(C,C'))
///
/// Example: F: int -> pair[int] is F(x) = make_pair(x,x)
///   where each arithmetic operation shall also be defined on pairs
template <template <class, int> class C /*< container class*/, class T, class U>
class Functor {
  public:
    /// Apply the function F to the given elements pϵT
    template <function<U, T> F, class... P>
    static constexpr C<U, sizeof...(P)> apply(P... p) {
        return C<U, sizeof...(P)>{F(static_cast<T>(p))...};
    }
};

/// A natural transformation A: F->G is a set of morphisms {A(t): F(t)->G(t) |
/// tϵOb(C)} that satisfies:
///   1. A(u)∘F(h) = G(h)∘A(t)
///   where t,uϵOb(C), h:t->uϵHomC(t,u) and F,G:C->C'ϵHomCat(C,C')
/// (Note: in the category of functors `Fun`(C,C'), functors C->C' are the
/// objects Ob(Fun) and natural tranformations are the morphisms HomFun(F,G))
///
///   Example: F[int->pair<int,int>], G[int->pair<int>],
///   h: pair<int>->pair<int> = ({x1,x2},{y1,y2})={x1,y1}
///     Note: int and pair<int> are both monoidal under arithmetic operations
///     A_#: f(#)->g(#) must satisfy g(#)=A_#(h(f(#)))=h(A_#(f(#)))
///     E.g. {9,9}=A_#(f(3))=f(A_#(3)) with A_#(x)={x,x}
template <template <class, int> class C, class T1, class T2, class T3,
          function<Functor<C, T2, T3>, Functor<C, T1, T2>> A>
class Nat {
  public:
    constexpr Functor<C, T2, T3> operator()(Functor<C, T1, T2> p) const {
        return A(p);
    }
};

template <class T> class Monad {
    T value_;

  public:
};

constexpr int add(int a, int b) { return a + b; }
using NumAdd = Monoid<int, 0, add>;
constexpr int square(int a) { return a * a; }
using NumF = Functor<array, int, int>;
template <class T> constexpr pair<T, T> replicate(T p) { return {p, p}; }
using NumPairF = Functor<array, int, pair<int, int>>;

static_assert(NumAdd(5)(6)(NumAdd::nil).value_ == NumAdd::nil + 11);
static_assert(NumF::apply<square>(2, 3) == array<int, 2>(4, 9));
static_assert(NumPairF::apply<replicate>(3, 2, 5) ==
              array<pair<int, int>, 3>(pair<int, int>{3, 3},
                                       pair<int, int>{2, 2},
                                       pair<int, int>{5, 5}));
