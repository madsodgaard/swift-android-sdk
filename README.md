# Swift Android SDK for API 23+

This is *just* a fork of [this Android SDK](https://github.com/finagolfin/swift-android-sdk),
modified to support API 23 as well. Most of the work should therefor be credited to the contributors of that project.

## Cross-compiling and testing Swift packages with the Android SDK bundle

To build with the Swift 6.2 SDK bundle, first download [the official open-source
Swift 6.2 toolchain for linux or macOS](https://swift.org/install)
(make sure to install the Swift dependencies linked there). Install the OSS
toolchain on macOS as detailed in [the instructions for using the static linux
Musl SDK bundle at swift.org](https://www.swift.org/documentation/articles/static-linux-getting-started.html).
On linux, simply download the toolchain, unpack it, and add it to your `PATH`.

Next, install the Android SDK bundle by having the Swift toolchain directly
download it:
```
swift sdk install https://github.com/finagolfin/swift-android-sdk/releases/download/6.2/swift-6.2-RELEASE-android-24-0.1.artifactbundle.tar.gz --checksum c26ebfd4e32c0ca1beabcc45729b62042da57ee76d7d043f63f2235da90dc491
```
You can check if it was properly installed by running `swift sdk list`.

Now you're ready to cross-compile a Swift package and run its tests on Android.
I'll demonstrate with the swift-argument-parser package:
```
git clone --depth 1 https://github.com/apple/swift-argument-parser.git

cd swift-argument-parser/

swift build --swift-sdk aarch64-unknown-linux-android23
```

You can copy these executables and the Swift runtime libraries to [an emulator
or a USB debugging-enabled device with adb](https://github.com/swiftlang/swift/blob/release/6.2/docs/Android.md#3-deploying-the-build-products-to-the-device).

## Building an Android app with Swift

Some people have reported an issue with using previous libraries from this SDK in
their Android app, that the Android toolchain strips `libdispatch.so` and
complains that it has an `empty/missing DT_HASH/DT_GNU_HASH`. You can [work
around this issue by adding the following to your `build.gradle`](https://github.com/finagolfin/swift-android-sdk/issues/67#issuecomment-1227460068):
```
packagingOptions {
    doNotStrip "*/arm64-v8a/libdispatch.so"
    doNotStrip "*/armeabi-v7a/libdispatch.so"
    doNotStrip "*/x86_64/libdispatch.so"
}
```

## Limitations


## Changes
Below is a list of the changes we had to make to make it work on API 23.

### Make `FILE` import as OpaquePointer
On Android 23 the `FILE` struct was actually visible, meaning that it would get
"correctly" imported as `UnsafeMutablePointer<FILE>`. However, on API 24+
it is imported as `OpaquePointer`. Since a lot of Swift libraries do
```swift
if #canImport(Android)
typealias CFilePointer = OpaquePointer
#else
typealias CFilePointer = UnsafeMutablePointer<FILE>
#endif
```
it would fail to build on API 23. We therefore path the `stdio.h` NDK header
to make sure it is imported as `OpaquePointer` on API 23 as well.

### `ifaddrs.h` stub
`getifaddrs` and `freeifaddrs` was added in API 24.
We therefore add a shim for this on Android 23. The implementation
is basically the [Android implementation](https://android.googlesource.com/platform/bionic/+/refs/tags/ndk-r29/libc/bionic/ifaddrs.cpp),
but modified to make it compile with the Swift standard library.

We also modify the `ifaddrs.h` header to move the constraint to API 23 instead of 24.

### Bionic group file APIs
Android 23 does not include support for `getgrgid_r` and `getgrnam_r`,
which are used in `FoundationEssentials/Platform.swift`.
We have just stubbed these out, as we don't need them.

### Disable backtrace in swift-testing
`swift-testing` uses `backtrace`, which does not work on Android 23.
We therefore just disable backtrace support.