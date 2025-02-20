FROM ubuntu:24.04

# Set noninteractive mode for apt
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    software-properties-common \
    curl \
    git \
    wget \
    unzip \
    ccache \
    build-essential \
    ninja-build \
    openjdk-21-jdk \
    gitpod-cli \
    && rm -rf /var/lib/apt/lists/*

# Add Kitware's APT repository and install latest CMake
RUN wget -qO - https://apt.kitware.com/keys/kitware-archive-latest.asc | apt-key add - && \
    add-apt-repository 'deb https://apt.kitware.com/ubuntu/ focal main' && \
    apt-get update && apt-get install -y cmake && \
    rm -rf /var/lib/apt/lists/*

# Install SDKMAN! and use it for GraalVM
RUN curl -s "https://get.sdkman.io" | bash && \
    bash -c "source /root/.sdkman/bin/sdkman-init.sh && sdk install java 21.0.5-graalce"
ENV JAVA_HOME="/root/.sdkman/candidates/java/current"
ENV PATH="$JAVA_HOME/bin:$PATH"

# Install Android SDK and NDK using sdkmanager
RUN mkdir -p /opt/android-sdk && \
    wget -q "https://dl.google.com/android/repository/commandlinetools-linux-11076708_latest.zip" -O /tmp/cmdline-tools.zip && \
    unzip /tmp/cmdline-tools.zip -d /opt/android-sdk/cmdline-tools && \
    mv /opt/android-sdk/cmdline-tools/cmdline-tools /opt/android-sdk/cmdline-tools/latest && \
    rm /tmp/cmdline-tools.zip
ENV ANDROID_SDK_ROOT="/opt/android-sdk"
ENV PATH="$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$ANDROID_SDK_ROOT/platform-tools:$PATH"
RUN yes | sdkmanager --licenses && \
    sdkmanager "ndk;28.0.13004108"
ENV ANDROID_NDK_HOME="$ANDROID_SDK_ROOT/ndk/28.0.13004108"
ENV PATH="$ANDROID_NDK_HOME:$PATH"

# Gitpod-specific setup
RUN echo "export PATH=$PATH" >> /home/gitpod/.bashrc && \
    echo "export JAVA_HOME=$JAVA_HOME" >> /home/gitpod/.bashrc && \
    echo "export ANDROID_SDK_ROOT=$ANDROID_SDK_ROOT" >> /home/gitpod/.bashrc && \
    echo "export ANDROID_NDK_HOME=$ANDROID_NDK_HOME" >> /home/gitpod/.bashrc

# Verify installations
RUN java -version && cmake --version && ccache --version && echo "Android NDK: $ANDROID_NDK_HOME"

CMD ["/bin/bash"]
