#pragma once

#include "OpenML.h"

namespace OpenML 
{
	/// <summary>
	/// Define a point in 2D space
	/// </summary>
	template <typename T>
	class Point2D
	{
	public:
		T x = T(0);
		T y = T(0);

		/// <summary>
		/// Default constructor
		/// </summary>
		API_INTERFACE
		Point2D();

		/// <summary>
		/// Constructor with args
		/// </summary>
		API_INTERFACE
		Point2D(T x, T y);

		/// <summary>
		/// Constructor from a vector, that means a point
		/// </summary>
		API_INTERFACE
		Point2D(Vec2<T> vector);

		/// <summary>
		/// Get the distance from 2 points
		/// </summary>
		API_INTERFACE
		T getDistance(Point2D<T> otherPoint);

		/// <summary>
		/// Convert to a vector
		/// </summary>
		API_INTERFACE
		Vec2<T> toVec2();

		/// <summary>
		/// Check the points are equals
		/// </summary>		
		bool API_INTERFACE operator==(const Point2D<T> &otherPoint) const;

		/// <summary>
		/// Check the points are different
		/// </summary>
		bool API_INTERFACE operator!=(const Point2D<T> &otherPoint) const;

		/// <summary>
		/// Convert to a vector
		/// </summary>
		API_INTERFACE
		operator Vec2<T>() const;

		/// <summary>
		/// Find the point that contain the minimum X value
		/// </summary>		
		static Point2D<T> API_INTERFACE findMinX(Point2D<T>* points, size_t pointsCount);

		/// <summary>
		/// Find the point that contain the minimum Y value
		/// </summary>		
		static Point2D<T> API_INTERFACE findMinY(Point2D<T>* points, size_t pointsCount);

		/// <summary>
		/// Find the point that contain the maximum X value
		/// </summary>		
		static Point2D<T> API_INTERFACE findMaxX(Point2D<T>* points, size_t pointsCount);

		/// <summary>
		/// Find the point that contain the maximum Y value
		/// </summary>		
		static Point2D<T> API_INTERFACE findMaxY(Point2D<T>* points, size_t pointsCount);

	};

	typedef Point2D<int> Point2Di;
	typedef Point2D<float> Point2Df;
	typedef Point2D<double> Point2Dd;
}