#import "MGLStyle.h"

#import "MGLStyleLayer.h"
#import "MGLFillStyleLayer.h"
#import <mbgl/util/default_styles.hpp>
#include <mbgl/mbgl.hpp>

@class MGLAttributionInfo;

@interface MGLStyle (Private)

- (instancetype)initWithMapView:(MGLMapView *)mapView;

@property (nonatomic, readonly, weak) MGLMapView *mapView;

@property (nonatomic, readonly) NS_ARRAY_OF(MGLAttributionInfo *) *attributionInfos;

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses transitionDuration:(NSTimeInterval)transitionDuration;

@end
