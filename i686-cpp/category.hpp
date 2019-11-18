#pragma once
#include "kernel/core.hpp"

/// A category C consists of:
///   1. a set of objects Ob(C)
///   2. for each pair of objects, a set of (homo)morphisms Hom(x,y)
///   3. for each object x, an identity morphism id(x) ϵ Hom(x,x)
///   4. a composition formula *: Hom(x,y),Hom(y,z)=Hom(x,z)
///   where multiple morphisms can be composed in any order
///     and the identity morphism is "neutral"

/// A monoid M[T,op,nil] consists of:
///   1. a singleton set Ob(M)={#}
///   2. a morphism Hom(#,#)=T
///   3. an identity id(#)=nil
///   4. a composition formula *(h1,h2)=op(h1,h2)
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
///   where each object x has an inverse such that op(x,x')=nil
template <class T, T N, function<T, T, T> F, function<T, T> I>
class Group : public Monoid<T, N, F> {
    using Monoid<T, N, F>::id;

  public:
    constexpr Group(T value) : Monoid<T, N, F>(value) {}
    constexpr Monoid<T, N, F> inv() const { return F(I(id())); }
};

/// A functor F[C->C'] consists of:
///   1. a function Ob(F)=f: Ob(C)->Ob(C')
///   2. for each pair t1,t2 ϵ Ob(C), a function HomF(t1,t2):
///   HomC(t1,t2)->HomC'(f(t1),f(t2))
///   so that f(id(x))=id(f(x)) and f(h1;h2(t))=f(h1(t));f(h2(t))
///
/// Example: F[int -> pair<int>] has f(x)=make_pair(x,x)
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

/// A natural transformation A[F->G] from F[C->C'] to G[C->C'] consists of:
///   1. for each t ϵ Ob(C), a morphism A_t: f(t)->g(t) ϵ C'->C'
///   (Reminder: f=Ob(F) and g=Ob(G))
///   where for each t,u ϵ Ob(C) and hom: C'->C' ϵ HomC'(f(t),f(u)),
///     we have g(u) = A_u(hom(f(t))) = hom(A_t(f(t)))
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
