#include "benchmark/benchmark.h"
#include "box2d.h"

#include <random>

void GenerateRandomBoxesAndSegments(std::vector<Box2d>* boxes,
                                    std::vector<Segment2d>* segments) {
  std::mt19937 generator;
  std::uniform_real_distribution<> box_xy_distribution(-10.0, 10.0);
  std::uniform_real_distribution<> box_heading_distribution(-M_PI, M_PI);
  std::uniform_real_distribution<> box_length_distribution(0.1, 10.0);
  std::uniform_real_distribution<> box_width_distribution(0.1, 10.0);
  std::uniform_real_distribution<> seg_xy_distribution(-20.0, 20.0);
  constexpr int kNumBoxes = 20;
  constexpr int kNumSegments = 50;
  for (int i = 0; i < kNumBoxes; ++i) {
    boxes->emplace_back(Vec2d(box_xy_distribution(generator), box_xy_distribution(generator)),
                        FromUnit(box_heading_distribution(generator)),
                        box_length_distribution(generator),
                        box_width_distribution(generator));
  }
  for (int i = 0; i < kNumSegments; ++i) {
    segments->emplace_back(Vec2d(seg_xy_distribution(generator), seg_xy_distribution(generator)),
        Vec2d(seg_xy_distribution(generator), seg_xy_distribution(generator)));
  }
}

void GenerateBoxAndSegmentPairs(std::vector<std::pair<Box2d, Segment2d>> *box_and_segment_pairs,
                                bool is_overlap) {
  std::mt19937 generator;
  std::uniform_real_distribution<> box_xy_distribution(-10.0, 10.0);
  std::uniform_real_distribution<> box_heading_distribution(-M_PI, M_PI);
  std::uniform_real_distribution<> box_length_distribution(0.1, 10.0);
  std::uniform_real_distribution<> box_width_distribution(0.1, 10.0);
  std::uniform_real_distribution<> seg_xy_distribution(-20.0, 20.0);
  constexpr int kNumPairs = 1000;
  while (box_and_segment_pairs->size() < kNumPairs) {
    Box2d box(Vec2d(box_xy_distribution(generator), box_xy_distribution(generator)),
              FromUnit(box_heading_distribution(generator)),
              box_length_distribution(generator),
              box_width_distribution(generator));
    Segment2d segment(Vec2d(seg_xy_distribution(generator), seg_xy_distribution(generator)),
        Vec2d(seg_xy_distribution(generator), seg_xy_distribution(generator)));
    if (box.HasOverlapWithSegment(segment) == is_overlap) {
      box_and_segment_pairs->emplace_back(box, segment);
    }
  }
}

void BM_DistanceSqrToSegment(benchmark::State& state) {  // NOLINT(runtime/references)
  std::vector<Box2d> boxes = {};
  std::vector<Segment2d> segments = {};
  GenerateRandomBoxesAndSegments(&boxes, &segments);
  for (auto _ : state) {
    for (const auto& box : boxes) {
      for (const auto& segment : segments) {
        benchmark::DoNotOptimize(box.DistanceSqrToSegment(segment));
      }
    }
  }
}
BENCHMARK(BM_DistanceSqrToSegment);

void BM_DistanceSqrToSegment_OverlapCases(benchmark::State& state) {  // NOLINT(runtime/references)
  std::vector<std::pair<Box2d, Segment2d>> box_and_segment_pairs = {};
  GenerateBoxAndSegmentPairs(&box_and_segment_pairs, true);
  for (auto _ : state) {
    for (const auto& [box, segment] : box_and_segment_pairs) {
      benchmark::DoNotOptimize(box.DistanceSqrToSegment(segment));
    }
  }
}
BENCHMARK(BM_DistanceSqrToSegment_OverlapCases);

void BM_DistanceSqrToSegment_NonOverlapCases(
    benchmark::State& state) {  // NOLINT(runtime/references)
  std::vector<std::pair<Box2d, Segment2d>> box_and_segment_pairs = {};
  GenerateBoxAndSegmentPairs(&box_and_segment_pairs, false);
  for (auto _ : state) {
    for (const auto& [box, segment] : box_and_segment_pairs) {
      benchmark::DoNotOptimize(box.DistanceSqrToSegment(segment));
    }
  }
}
BENCHMARK(BM_DistanceSqrToSegment_NonOverlapCases);

void BM_HasOverlapWithSegment_OverlapCases(benchmark::State& state) {  // NOLINT(runtime/references)
  std::vector<std::pair<Box2d, Segment2d>> box_and_segment_pairs = {};
  GenerateBoxAndSegmentPairs(&box_and_segment_pairs, true);
  for (auto _ : state) {
    for (const auto& [box, segment] : box_and_segment_pairs) {
      benchmark::DoNotOptimize(box.HasOverlapWithSegment(segment));
    }
  }
}
BENCHMARK(BM_HasOverlapWithSegment_OverlapCases);

void BM_HasOverlapWithSegment_NonOverlapCases(
    benchmark::State& state) {  // NOLINT(runtime/references)
  std::vector<std::pair<Box2d, Segment2d>> box_and_segment_pairs = {};
  GenerateBoxAndSegmentPairs(&box_and_segment_pairs, false);
  for (auto _ : state) {
    for (const auto& [box, segment] : box_and_segment_pairs) {
      benchmark::DoNotOptimize(box.HasOverlapWithSegment(segment));
    }
  }
}
BENCHMARK(BM_HasOverlapWithSegment_NonOverlapCases);

BENCHMARK_MAIN();
