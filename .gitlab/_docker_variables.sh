
# git repositories inside dockers
export wsgd_wireshark_repository=/wireshark/dev/windows/wireshark
export wsgd_wsgd_repository=/wireshark/dev/windows/generic/.git

export CI_COMMIT_TAG="debian-stable_2021.11.11"
export CI_COMMIT_TAG="centos-8_2021.11.13"
export CI_API_V4_URL="not_an_url"
export CI_PROJECT_ID="project_id"
export CI_JOB_TOKEN="JobToken"

export VERSIONS="304XX 302XX 300XX"
export PACKAGE_REGISTRY_URL="${CI_API_V4_URL}/projects/${CI_PROJECT_ID}/packages/generic/${CI_COMMIT_TAG}/"
