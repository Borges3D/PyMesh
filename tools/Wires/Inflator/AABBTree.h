#pragma once

#include <Core/EigenTypedef.h>

#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>
#include <CGAL/Cartesian.h>

#include <cassert>
#include <vector>
#include <memory>

class AABBTree {
    public:
        typedef std::shared_ptr<AABBTree> Ptr;

        typedef CGAL::Cartesian<Float> K;
        typedef K::Point_3 Point;
        typedef K::Triangle_3 Triangle;

        typedef std::vector<Triangle> Triangles;
        typedef CGAL::AABB_triangle_primitive<K, Triangles::iterator> Primitive;
        typedef CGAL::AABB_traits<K, Primitive> AABB_triangle_traits;
        typedef CGAL::AABB_tree<AABB_triangle_traits> Tree;
        typedef std::shared_ptr<Tree> TreePtr;
        typedef Tree::Point_and_primitive_id Point_and_primitive_id;

    public:
        AABBTree(const MatrixFr& vertices, const MatrixIr& faces) {
            assert(faces.cols() == 3);
            m_dim = vertices.cols();

            const size_t num_faces = faces.rows();
            for (size_t i=0; i<num_faces; i++) {
                const Vector3I& f = faces.row(i);
                m_triangles.emplace_back(
                        to_cgal_point(vertices.row(f[0])),
                        to_cgal_point(vertices.row(f[1])),
                        to_cgal_point(vertices.row(f[2])));
            }

            m_tree = std::make_shared<Tree>(m_triangles.begin(), m_triangles.end());
            m_tree->accelerate_distance_queries();
        }

        void look_up(const MatrixFr& points,
                VectorF& squared_dists,
                VectorI& closest_face_indices) {
            const size_t num_pts = points.rows();
            squared_dists.resize(num_pts);
            closest_face_indices.resize(num_pts);

            for (size_t i=0; i<num_pts; i++) {
                Point p = to_cgal_point(points.row(i));
                Point_and_primitive_id itr = m_tree->closest_point_and_primitive(p);
                closest_face_indices[i] = itr.second - m_triangles.begin();
                Point p2 = itr.first;
                squared_dists[i] = (p-p2).squared_length();
            }
        }

        void look_up_with_closest_points(const MatrixFr& points,
                VectorF& squared_dists,
                VectorI& closest_face_indices,
                MatrixFr& closest_points) {
            const size_t num_pts = points.rows();
            squared_dists.resize(num_pts);
            closest_face_indices.resize(num_pts);
            closest_points.resize(num_pts, m_dim);

            for (size_t i=0; i<num_pts; i++) {
                Point p = to_cgal_point(points.row(i));
                Point_and_primitive_id itr = m_tree->closest_point_and_primitive(p);
                closest_face_indices[i] = itr.second - m_triangles.begin();
                Point p2 = itr.first;
                closest_points.row(i) = to_eigen_point(p2);
                squared_dists[i] = (p-p2).squared_length();
            }
        }

    private:
        Point to_cgal_point(const VectorF& p) {
            if (m_dim == 2)
                return Point(p[0], p[1], 0.0);
            else
                return Point(p[0], p[1], p[2]);
        }

        VectorF to_eigen_point(const Point& p) {
            if (m_dim == 2)
                return Vector2F(p[0], p[1]);
            else
                return Vector3F(p[0], p[1], p[2]);
        }

    private:
        size_t m_dim;
        TreePtr m_tree;
        Triangles m_triangles;
};
