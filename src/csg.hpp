#ifndef CSG_H
#define CSG_H

#include "SETTINGS.hpp"
#include "material.hpp"
#include "shape.hpp"

class CSGSpan {
private:
    static bool hitLessThan(const Intersection& a, const Intersection& b) {
        return a.t < b.t;
    }

public:
    static Intersection getFirstWithCriteria(CSGSpan a, CSGSpan b, bool (*criteria)(bool, bool), bool debug) {
        // FIXME this is just begging for a better data structure
        // It's slow wrt complexity of surfaces, which could be an issue
        // with large CSG trees (have to compute for each intersection)

        Real min_t;
        Intersection min_i(Ray(Point(666,666,666), (Vector(0,0,0))));
        bool found = false;

        for (int i = 0; i < a.hits.size() + b.hits.size(); ++i) {
            Intersection& inter = (i < a.hits.size())? a.hits[i] : b.hits[i-a.hits.size()];

            bool inA = a.contains(inter.t);
            bool inB = b.contains(inter.t);

            bool valid = criteria(inA, inB);

            if (valid && (!found || inter.t < min_t)) {
                min_i = inter;
                min_t = min_i.t;
                found = true;
            }
        }

        return min_i;
    }

public:
    vector<Intersection> hits;

    CSGSpan() {};

    CSGSpan(const Intersection& i, const Shape* shape) {
        Intersection tester = Intersection(i);
        shape->intersect(tester);
        int count = 0;

        while(tester.intersected && count <= 5) {
            // Make intersection from camera for insertion
            tester.t = (tester.ray.at(tester.t) - i.ray.origin).dot(i.ray.dir);
            tester.ray = i.ray;
            insert(tester);

            // Move ray back to where it was, progress it a bit and intersect it
            Ray nextRay(tester.ray.at(tester.t + 2 * SURFACE_EPS), tester.ray.dir);
            tester = Intersection(nextRay);
            shape->intersect(tester);
            count++;
        }
    }

    void insert(const Intersection& i) {
        vector<Intersection>::iterator it = lower_bound(hits.begin(), hits.end(), i, hitLessThan);
        hits.insert(it, i);
        // hits.insert(hits.end(), i);
    }

    bool contains(Real t) {
        bool out = false;
        for (int i = 0; i < hits.size(); ++i) {
            if (hits[i].t > t) return out;
            out = !out;
        }
        return out;
    }

    void print() {
        printf("SPAN {\n");
        for (int i = 0; i < hits.size(); ++i) {
            printf("\t");
            hits[i].print();
        }
        printf("}\n");
    }

};


class CSGOperator: public Shape {
private:
    Shape *a;
    Shape *b;
    bool(*criteria)(bool, bool);
public:
    CSGOperator(Shape *a, Shape *b, bool(*criteria)(bool, bool)): a(a), b(b), criteria(criteria) {}

    bool intersect(Intersection& i) const {
        //FIXME Account for intersection from inside, sending ray backwards and merging spans
        if (this->getBoundingBox().contains(i.ray.o)) return false;

        if(not this->getBoundingBox().doesIntersect(i.ray)) return false;

        CSGSpan a_span(i, a);
        CSGSpan b_span(i, b);

        if(i.DEBUG) {
            a_span.print();
            b_span.print();
        }

        Intersection out = CSGSpan::getFirstWithCriteria(a_span, b_span, this->criteria, i.DEBUG);

        if (!out.intersected) return false;
        if (i.intersected && out.t > i.t) return false;

        // Copy over everything but the ray
        // TODO Make this copying a method of Intersection class

        i.intersected = out.intersected; // Always true

        i.shape = out.shape;

        i.normal = out.normal;

        i.t = out.t;
        i.u = out.u;
        i.v = out.v;

        i.tangent = out.tangent;
        i.bitangent = out.bitangent;

        return true;
    }

    void setMaterial(Material* mat) {
        a->setMaterial(mat);
        b->setMaterial(mat);
    }

    void translate(const Vector& t){
        a->translate(t);
        b->translate(t);
    }

    void rotate(const Vector& axis, const Real angle) {
        a->rotate(axis, angle);
        b->rotate(axis, angle);
    }

    AABB getBoundingBox() const {
        Point min = a->getBoundingBox().min().cwiseMin(b->getBoundingBox().min());
        Point max = a->getBoundingBox().max().cwiseMax(b->getBoundingBox().max());
        return AABB(min, max);
    }

    void updateBoundingBox() {
        a->updateBoundingBox();
        b->updateBoundingBox();
        this->boundingBox = getBoundingBox();
    }

};

class CSGCriteria {
public:
    static bool Intersection(bool a, bool b) {
        return a && b;
    }

    static bool Union(bool a, bool b) {
        return a || b;
    }

    static bool Difference(bool a, bool b) {
        return (a && !b);
    }
};

struct CSGIntersection: CSGOperator {
    CSGIntersection(Shape *a, Shape *b): CSGOperator(a, b, CSGCriteria::Intersection) {}
};

struct CSGDifference: CSGOperator {
    CSGDifference(Shape *a, Shape *b): CSGOperator(a, b, CSGCriteria::Difference) {}
};

struct CSGUnion: CSGOperator {
    CSGUnion(Shape *a, Shape *b): CSGOperator(a, b, CSGCriteria::Union) {}
};

#endif
