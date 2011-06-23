//
// Copyright 2011 Jeff Verkoeyen
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @brief Nimbus' Core library contains many often used utilities.
 * @defgroup NimbusCore Nimbus Core
 * @{
 *
 * The Nimbus Core sets the foundation for all of Nimbus' other libraries. By establishing a
 * strong base of helpful utility methods and debugging tools, the rest of the libraries can
 * benefit from this code reuse and decreased time spent re-inventing the wheel.
 *
 * In your own projects, consider familiarizing yourself with Nimbus by first adding the
 * Core and feeling your way around. For existing projects this is especially recommended
 * because it allows you to gradually introduce concepts found within Nimbus.
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>


#pragma mark -
#pragma mark Preprocessor Macros

/**
 * @brief For writing safer code.
 * @defgroup Preprocessor-Macros Preprocessor Macros
 * @{
 */

/**
 * @brief Mark a method or property as deprecated to the compiler.
 *
 * Any use of a deprecated method or property will flag a warning when compiling.
 *
 * Borrowed from Apple's AvailabiltyInternal.h header.
 *
 * @htmlonly
 * <pre>
 *   __AVAILABILITY_INTERNAL_DEPRECATED         __attribute__((deprecated))
 * </pre>
 * @endhtmlonly
 */
#define __NI_DEPRECATED_METHOD __attribute__((deprecated))

/**
 * @brief Force a category to be loaded when an app starts up.
 *
 * Add this macro before each category implementation, so we don't have to use
 * -all_load or -force_load to load object files from static libraries that only contain
 * categories and no classes.
 * See http://developer.apple.com/library/mac/#qa/qa2006/qa1490.html for more info.
 */
#define NI_FIX_CATEGORY_BUG(name) @interface NI_FIX_CATEGORY_BUG_##name @end \
                                  @implementation NI_FIX_CATEGORY_BUG_##name @end

/**
 * @brief Release and assign nil to an object.
 *
 * This macro is preferred to simply releasing an object to avoid accidentally using the
 * object later on in a method.
 */
#define NI_RELEASE_SAFELY(__POINTER) { [__POINTER release]; __POINTER = nil; }


///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of Preprocessor Macros //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark -
#pragma mark Debugging Tools

/**
 * @brief For inspecting code and writing to logs in debug builds.
 * @defgroup Debugging-Tools Debugging Tools
 * @{
 *
 * Provided in this header are a set of debugging methods and macros. Nearly all of the methods
 * found within this header will only do anything interesting if the DEBUG macro is defined.
 * The recommended way to enable the debug tools is to specify DEBUG in the "Preprocessor Macros"
 * field in your application's Debug target settings. Be careful not to set this for your release
 * or app store builds.
 *
 * @code
 * NIDASSERT(statement);
 * @endcode
 *
 * If statement is false, the statement will be written to the log and if you are running in
 * the simulator with a debugger attached, the app will break on the assertion line.
 *
 * @code
 * NIDPRINT(@"formatted log text %d", param1);
 * @endcode
 *
 * Print the given formatted text to the log.
 *
 * @code
 * NIDPRINTMETHODNAME();
 * @endcode
 *
 * Print the current method name to the log.
 *
 * @code
 * NIDCONDITIONLOG(statement, @"formatted log text %d", param1);
 * @endcode
 *
 * If statement is true, then the formatted text will be written to the log.
 *
 * @code
 * NIDINFO/NIDWARNING/NIDERROR(@"formatted log text %d", param1);
 * @endcode
 *
 * Will only write the formatted text to the log if NIMaxLogLevel is greater than the respective
 * NID* method's log level. See below for log levels.
 *
 * The default maximum log level is NILOGLEVEL_WARNING.
 */

#define NILOGLEVEL_INFO     5
#define NILOGLEVEL_WARNING  3
#define NILOGLEVEL_ERROR    1

/**
 * @brief The maximum log level to output for Nimbus debug logs.
 *
 * This value may be changed at run-time if you so desire.
 *
 * The default value is NILOGLEVEL_WARNING.
 */
extern NSInteger NIMaxLogLevel;

/**
 * @brief Only writes to the log when DEBUG is defined.
 *
 * This log method will always write to the log, regardless of log levels. It is used by all
 * of the other logging methods in Nimbus' debugging library.
 */
#ifdef DEBUG
#define NIDPRINT(xx, ...)  NSLog(@"%s(%d): " xx, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define NIDPRINT(xx, ...)  ((void)0)
#endif // #ifdef DEBUG

/**
 * @brief Write the containing method's name to the log using NIDPRINT.
 */
#define NIDPRINTMETHODNAME() NIDPRINT(@"%s", __PRETTY_FUNCTION__)

/**
 * @brief Assertions that only fire when DEBUG is defined.
 *
 * An assertion is like a programmatic breakpoint. Use it for sanity checks to save headache while
 * writing your code.
 */
#ifdef DEBUG

#import <TargetConditionals.h>

#if TARGET_IPHONE_SIMULATOR
int NIIsInDebugger();
// We leave the __asm__ in this macro so that when a break occurs, we don't have to step out of
// a "breakInDebugger" function.
#define NIDASSERT(xx) { if (!(xx)) { NIDPRINT(@"NIDASSERT failed: %s", #xx); \
if (NIIsInDebugger()) { __asm__("int $3\n" : : ); }; } \
} ((void)0)
#else
#define NIDASSERT(xx) { if (!(xx)) { NIDPRINT(@"NIDASSERT failed: %s", #xx); } } ((void)0)
#endif // #if TARGET_IPHONE_SIMULATOR

#else
#define NIDASSERT(xx) ((void)0)
#endif // #ifdef DEBUG

#ifdef DEBUG
#define NIDCONDITIONLOG(condition, xx, ...) { if ((condition)) { NIDPRINT(xx, ##__VA_ARGS__); } \
} ((void)0)
#else
#define NIDCONDITIONLOG(condition, xx, ...) ((void)0)
#endif // #ifdef DEBUG


#pragma mark Level-Based Loggers

/**
 * @name Level-Based Loggers
 * @{
 */

/**
 * @brief Only writes to the log if NIMaxLogLevel >= NILOGLEVEL_ERROR.
 */
#define NIDERROR(xx, ...)  NIDCONDITIONLOG((NILOGLEVEL_ERROR <= NIMaxLogLevel), xx, ##__VA_ARGS__)

/**
 * @brief Only writes to the log if NIMaxLogLevel >= NILOGLEVEL_WARNING.
 */
#define NIDWARNING(xx, ...)  NIDCONDITIONLOG((NILOGLEVEL_WARNING <= NIMaxLogLevel), \
                                             xx, ##__VA_ARGS__)

/**
 * @brief Only writes to the log if NIMaxLogLevel >= NILOGLEVEL_INFO.
 */
#define NIDINFO(xx, ...)  NIDCONDITIONLOG((NILOGLEVEL_INFO <= NIMaxLogLevel), xx, ##__VA_ARGS__)

/**@}*/// End of Level-Based Loggers


///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of Debugging Tools //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark -
#pragma mark Data Structures

/**
 * @brief For data structures.
 * @defgroup Data-Structures Data Structures
 * @{
 */

struct NILinkedListNode {
  id    object;
  struct NILinkedListNode* prev;
  struct NILinkedListNode* next;
};

typedef void NILinkedListLocation;

/**
 * @brief A linked list implementation.
 *
 * This data structure is provided for constant time insertion and deletion of objects
 * in a collection.
 */
@interface NILinkedList : NSObject <NSCopying, NSCoding, NSFastEnumeration> {
@private
  struct NILinkedListNode* _head;
  struct NILinkedListNode* _tail;
  unsigned long _count;

  // Used internally to track modifications to the linked list.
  unsigned long _modificationNumber;
}

/**
 * @brief The first object in the linked list.
 */
@property (nonatomic, readonly) id head;

/**
 * @brief The last object in the linked list.
 */
@property (nonatomic, readonly) id tail;

/**
 * @brief The number of objects in the linked list.
 */
@property (nonatomic, readonly) unsigned long count;

/**
 * @brief Designated initializer.
 */
- (id)init;

/**
 * @brief Convenience method for creating an autoreleased linked list.
 *
 * Identical to [[[NILinkedList alloc] init] autorelease];
 */
+ (NILinkedList *)linkedList;

/**
 * @brief Append an object to the linked list.
 *
 * Run-time: O(1)
 *
 * @returns A location within the linked list.
 */
- (NILinkedListLocation *)addObject:(id)object;

/**
 * @brief Retrieve the object at a specific location.
 *
 * Run-time: O(1)
 */
- (id)objectAtLocation:(NILinkedListLocation *)location;

/**
 * @brief Remove all objects from the linked list.
 *
 * Run-time: Theta(count)
 */
- (void)removeAllObjects;

/**
 * @brief Remove an object from the linked list.
 *
 * Run-time: O(count)
 */
- (void)removeObject:(id)object;

/**
 * @brief Remove the first object from the linked list.
 *
 * Run-time: O(1)
 */
- (void)removeFirstObject;

/**
 * @brief Remove the last object from the linked list.
 *
 * Run-time: O(1)
 */
- (void)removeLastObject;


/**
 * @name Pseudo-index support
 * The following methods are provided to aid in accessing objects in the linked list in constant
 * time.
 * @{
 */
#pragma mark Pseudo-index support

/**
 * @brief Search for an object in the linked list.
 *
 * Run-time: O(count)
 *
 * @returns A location within the linked list.
 */
- (NILinkedListLocation *)locationOfObject:(id)object;

/**
 * @brief Remove an object at a predetermined location.
 *
 * Run-time: O(1)
 *
 * It is assumed that this location still exists in the linked list. If the object this
 * location refers to has since been removed then this method will have undefined results.
 *
 * This is provided as an optimization over the O(n) removal method but should be used with care.
 */
- (void)removeObjectAtLocation:(NILinkedListLocation *)location;

/**@}*/// End of Pseudo-index support

@end



///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of Data Structures //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark -
#pragma mark Non-Retaining Collections

/**
 * @brief For collections that don't retain their objects.
 * @defgroup Non-Retaining-Collections Non-Retaining Collections
 * @{
 *
 * Non-retaining collections have historically been used when we needed more than one delegate
 * in an object. However, NSNotificationCenter is a much better solution for n > 1 delegates.
 * Using a non-retaining collection is dangerous, so if you must use one, use it with extreme care.
 * The danger primarily lies in the fact that by all appearances the collection should still
 * operate like a regular collection, so this might lead to a lot of developer error if the
 * developer assumes that the collection does, in fact, retain the object.
 */

/**
 * @brief Creates a mutable array which does not retain references to the objects it contains.
 *
 * Typically used with arrays of delegates.
 */
NSMutableArray* NICreateNonRetainingArray();


/**
 * @brief Creates a mutable dictionary which does not retain references to the values it contains.
 *
 * Typically used with dictionaries of delegates.
 */
NSMutableDictionary* NICreateNonRetainingDictionary();

/**
 * @brief Creates a mutable set which does not retain references to the values it contains.
 *
 * Typically used with sets of delegates.
 */
NSMutableSet* NICreateNonRetainingSet();


///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of Non-Retaining Collections ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark -
#pragma mark Non-Empty Collection Testing

/**
 * @brief For testing whether a collection is of a certain type and is non-empty.
 * @defgroup Non-Empty-Collection-Testing Non-Empty Collection Testing
 * @{
 *
 * Simply calling -count on an object may not yield the expected results when enumerating it if
 * certain assumptions are also made about the object's type. For example, if a JSON response
 * returns a dictionary when you expected an array, casting the result to an NSArray and
 * calling count will yield a positive value, but objectAtIndex: will crash the application.
 * These methods provide a safer check for non-emptiness of collections.
 */

/**
 * @brief Tests if an object is a non-nil array which is not empty.
 */
BOOL NIIsArrayWithObjects(id object);

/**
 * @brief Tests if an object is a non-nil set which is not empty.
 */
BOOL NIIsSetWithObjects(id object);

/**
 * @brief Tests if an object is a non-nil string which is not empty.
 */
BOOL NIIsStringWithAnyText(id object);


///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of Non-Empty Collection Testing /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark -
#pragma mark Runtime Class Modifications

/**
 * @brief For modifying class implementations at runtime.
 * @defgroup Runtime-Class-Modifications Runtime Class Modifications
 * @{
 *
 * @attention Please use caution when using modifying class implementations at runtime.
 * Apple is prone to rejecting apps for gratuitous use of method swapping. In particular,
 * avoid swapping any NSObject methods such as dealloc, init, and retain/release on UIKit classes.
 *
 * See example: @link ExampleRuntimeDebugging.m Runtime Debugging with Method Swizzling@endlink
 */

/**
 * @brief Swap two class instance method implementations.
 *
 * Use this method when you would like to replace an existing method implementation in a class
 * with your own implementation at runtime. In practice this is often used to replace the
 * implementations of UIKit classes where subclassing isn't an adequate solution.
 *
 * This will only work for methods declared with a -.
 *
 * After calling this method, any calls to originalSel will actually call newSel and vice versa.
 *
 * Uses method_exchangeImplementations to accomplish this.
 */
void NISwapInstanceMethods(Class cls, SEL originalSel, SEL newSel);

/**
 * @brief Swap two class method implementations.
 *
 * Use this method when you would like to replace an existing method implementation in a class
 * with your own implementation at runtime. In practice this is often used to replace the
 * implementations of UIKit classes where subclassing isn't an adequate solution.
 *
 * This will only work for methods declared with a +.
 *
 * After calling this method, any calls to originalSel will actually call newSel and vice versa.
 *
 * Uses method_exchangeImplementations to accomplish this.
 */
void NISwapClassMethods(Class cls, SEL originalSel, SEL newSel);


///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of Runtime Class Modifications //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark -
#pragma mark CGRect Methods

/**
 * @brief For manipulating CGRects.
 * @defgroup CGRect-Methods CGRect Methods
 * @{
 *
 * These methods provide additional means of modifying the edges of CGRects beyond the basics
 * included in CoreGraphics.
 */

/**
 * @brief Modifies only the right and bottom edges of a CGRect.
 * @return a CGRect with dx and dy subtracted from the width and height.
 *
 * Example result: CGRectMake(x, y, w - dx, h - dy)
 */
CGRect NIRectContract(CGRect rect, CGFloat dx, CGFloat dy);

/**
 * @brief Modifies only the top and left edges of a CGRect.
 * @return a CGRect whose origin has been offset by dx, dy, and whose size has been
 * contracted by dx, dy.
 *
 * Example result: CGRectMake(x + dx, y + dy, w - dx, h - dy)
 */
CGRect NIRectShift(CGRect rect, CGFloat dx, CGFloat dy);

/**
 * @brief Add the insets to a CGRect - equivalent to padding in CSS.
 * @return a CGRect whose edges have been inset.
 *
 * Example result: CGRectMake(x + left, y + top, w - (left + right), h - (top + bottom))
 */
CGRect NIRectInset(CGRect rect, UIEdgeInsets insets);


///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of CGRect Methods ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark -
#pragma mark Device Orientation

/**
 * @brief For dealing with device orientations.
 * @defgroup Device-Orientation Device Orientation
 * @{
 */

/**
 * @brief For use in shouldAutorotateToInterfaceOrientation:
 *
 * On iPhone/iPod touch:
 * Returns YES if the orientation is portrait, landscape left, or landscape right.
 * This helps to ignore upside down and flat orientations.
 *
 * On iPad:
 * Always returns YES.
 */
BOOL NIIsSupportedOrientation(UIInterfaceOrientation orientation);

/**
 * @brief Returns the application's current interface orientation.
 *
 * @returns The current interface orientation.
 */
UIInterfaceOrientation NIInterfaceOrientation();


///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of Device Orientation ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark -
#pragma mark Paths

/**
 * @brief For creating standard system paths.
 * @defgroup Paths Paths
 * @{
 */

/**
 * @brief Create a path with the given bundle and the relative path appended.
 *
 * @param bundle        The bundle to append relativePath to. If nil, [NSBundle mainBundle] will
 *                      be used.
 * @param relativePath  The relative path to append to the bundle's path.
 *
 * @returns The bundle path concatenated with the given relative path.
 */
NSString* NIPathForBundleResource(NSBundle* bundle, NSString* relativePath);

/**
 * @brief Create a path with the documents directory and the relative path appended.
 * @returns The documents path concatenated with the given relative path.
 */
NSString* NIPathForDocumentsResource(NSString* relativePath);


///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of Paths ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark -
#pragma mark SDK Availability

/**
 * @brief For checking SDK feature availibility.
 * @defgroup SDK-Availability SDK Availability
 * @{
 *
 * NIIOS macros are defined in parallel to their __IPHONE_ counterparts as a consistently-defined
 * means of checking __IPHONE_OS_VERSION_MAX_ALLOWED.
 *
 * For example:
 *
 * @htmlonly
 * <pre>
 *     #if __IPHONE_OS_VERSION_MAX_ALLOWED >= NIIOS_3_2
 *       // This code will only compile on versions >= iOS 3.2
 *     #endif
 * </pre>
 * @endhtmlonly
 */

/**
 * @brief Released on July 11, 2008
 */
#define NIIOS_2_0     20000

/**
 * @brief Released on September 9, 2008
 */
#define NIIOS_2_1     20100

/**
 * @brief Released on November 21, 2008
 */
#define NIIOS_2_2     20200

/**
 * @brief Released on June 17, 2009
 */
#define NIIOS_3_0     30000

/**
 * @brief Released on September 9, 2009
 */
#define NIIOS_3_1     30100

/**
 * @brief Released on April 3, 2010
 */
#define NIIOS_3_2     30200

/**
 * @brief Released on June 21, 2010
 */
#define NIIOS_4_0     40000

/**
 * @brief Released on September 8, 2010
 */
#define NIIOS_4_1     40100

/**
 * @brief Released on November 22, 2010
 */
#define NIIOS_4_2     40200

/**
 * @brief Released on March 9, 2011
 */
#define NIIOS_4_3     40300

/**
 * @brief Release TBD.
 */
#define NIIOS_5_0     50000

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_2_0
#define kCFCoreFoundationVersionNumber_iPhoneOS_2_0 478.23
#endif

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_2_1
#define kCFCoreFoundationVersionNumber_iPhoneOS_2_1 478.26
#endif

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_2_2
#define kCFCoreFoundationVersionNumber_iPhoneOS_2_2 478.29
#endif

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_3_0
#define kCFCoreFoundationVersionNumber_iPhoneOS_3_0 478.47
#endif

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_3_1
#define kCFCoreFoundationVersionNumber_iPhoneOS_3_1 478.52
#endif

#ifndef kCFCoreFoundationVersionNumber_iPhoneOS_3_2
#define kCFCoreFoundationVersionNumber_iPhoneOS_3_2 478.61
#endif

#ifndef kCFCoreFoundationVersionNumber_iOS_4_0
#define kCFCoreFoundationVersionNumber_iOS_4_0 550.32
#endif

/**
 * @brief Checks whether the device the app is currently running on is an iPad or not.
 *
 * @returns YES if the device is an iPad.
 */
BOOL NIIsPad();

/**
 * @brief Checks whether the device's OS version is at least the given version number.
 *
 * @param versionNumber Any value of kCFCoreFoundationVersionNumber.
 *
 * Useful for runtime checks of the device's version number.
 *
 * @attention Apple recommends using respondsToSelector where possible to check for
 * feature support. Use this method as a last resort.
 */
BOOL NIDeviceOSVersionIsAtLeast(double versionNumber);

/**
 * @brief Safely fetch the UIPopoverController class if it is available.
 *
 * The class is cached to avoid repeated lookups.
 *
 * Uses NSClassFromString to fetch the popover controller class.
 *
 * @attention If you wish to maintain pre-iOS 3.2 support then you <b>must</b> use this method
 * instead of directly referring to UIPopoverController anywhere within your code. Failure to
 * do so will cause your app to crash on startup on pre-iOS 3.2 devices.
 */
Class NIUIPopoverControllerClass();


///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of SDK Availability /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#pragma mark -
#pragma mark In-Memory Caches

/**
 * @brief For storing and accessing objects in memory.
 * @defgroup In-Memory-Caches In-Memory Caches
 * @{
 *
 * The base class, NIMemoryCache, is a generic object store that may be used for anything that
 * requires support for expiration.
 */

/**
 * @brief An in-memory cache for storing objects with expiration support.
 *
 * The Nimbus in-memory object cache allows you to store objects in memory with an expiration
 * date attached. Objects with expiration dates drop out of the cache when they have expired.
 */
@interface NIMemoryCache : NSObject {
@private
  // Mapping from a name (usually a URL) to an internal object.
  NSMutableDictionary*  _cacheMap;
}

/**
 * @brief Initialize the cache with no initial capacity.
 */
- (id)init;

/**
 * @brief Designated initializer. Initialize the cache with an initial capacity.
 *
 * Use a best guess to avoid having the internal data structure reallocate its memory repeatedly
 * - at least up up to a certain point - as the cache grows.
 */
- (id)initWithCapacity:(NSUInteger)capacity;

/**
 * @brief Store an object in the cache.
 *
 * @param object  The object being stored in the cache.
 * @param name    The name used as a key to store this object.
 *
 * The object will be stored without an expiration date. The object will stay in the cache until
 * it's bumped out due to the cache's memory limit.
 */
- (void)storeObject:(id)object withName:(NSString *)name;

/**
 * @brief Store an object in the cache with an expiration date.
 *
 * @param object          The object being stored in the cache.
 * @param name            The name used as a key to store this object.
 * @param expirationDate  A date after which this object is no longer valid in the cache.
 *
 * If an object is stored with an expiration date that has already passed then the object will
 * not be stored in the cache and any existing object will be removed. The rationale behind this
 * is that the object would be removed from the cache the next time it was accessed anyway.
 */
- (void)storeObject:(id)object withName:(NSString *)name expiresAfter:(NSDate *)expirationDate;

/**
 * @brief Retrive an object from the cache.
 *
 * If the object has expired then the object will be removed from the cache and nil will be
 * returned.
 */
- (id)objectWithName:(NSString *)name;

/**
 * @brief Remove an object in the cache.
 *
 * @param name The name used as a key to store this object.
 */
- (void)removeObjectWithName:(NSString *)name;

/**
 * @brief Remove all objects from the cache, regardless of expiration dates.
 *
 * This will completely clear out the cache and all objects in the cache will be released.
 */
- (void)removeAllObjects;

/**
 * @brief Remove all expired objects from the cache.
 *
 * This is meant to be used when a memory warning is received. Subclasses may add additional
 * functionality to this implementation.
 */
- (void)reduceMemoryUsage;

/**
 * @brief The number of objects stored in this cache.
 */
@property (nonatomic, readonly) NSUInteger count;


/**
 * @name Subclassing
 * The following methods are provided to aid in subclassing and are not meant to be
 * used externally.
 * @{
 */
#pragma mark Subclassing

/**
 * @brief An object is about to be stored in the cache.
 *
 * @param object          The object to be stored in the cache.
 * @param name            The cache name for the object.
 * @param previousObject  The object previously stored in the cache. This may be the same as object.
 */
- (void)willSetObject:(id)object withName:(NSString *)name previousObject:(id)previousObject;

/**
 * @brief An object is about to be removed from the cache.
 *
 * @param object  The object about to removed from the cache.
 * @param name    The cache name for the object about to be removed.
 */
- (void)willRemoveObject:(id)object withName:(NSString *)name;

/**@}*/

@end


/**
 * @brief An in-memory cache for storing images with a least-recently-used memory cap.
 */
@interface NIImageMemoryCache : NIMemoryCache {
@private
  NSUInteger _totalMemoryUsage;

  NSUInteger _maxTotalMemoryUsage;
  NSUInteger _maxTotalLowMemoryUsage;
}

/**
 * @brief The maximum amount of memory this cache may ever use.
 *
 * Defaults to 0, which is special cased to represent an unbounded cache size.
 */
@property (nonatomic, readwrite, assign) NSUInteger maxTotalMemoryUsage;

/**
 * @brief The maximum amount of memory this cache may use after a call to reduceMemoryUsage.
 *
 * Defaults to 0, which is special cased to represent an unbounded cache size.
 */
@property (nonatomic, readwrite, assign) NSUInteger maxTotalLowMemoryUsage;

@end




///////////////////////////////////////////////////////////////////////////////////////////////////
/**@}*/// End of In-Memory Cache //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


/**@}*/
