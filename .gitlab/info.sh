
echo
echo ">>> pwd :"
pwd

echo
echo ">>> uname -a :"
uname -a

echo
echo ">>> cat /etc/*-release :"
cat /etc/*-release

echo
echo ">>> g++ -v :"
g++ -v

echo
echo ">>> make --version :"
make --version

echo
echo ">>> cmake --version :"
cmake --version

echo
echo ">>> ls -a :"
ls -a

echo
echo ">>> git log --pretty=oneline --decorate | head -n 1 :"
git log --pretty=oneline --decorate | head -n 1

# Avoid gitlab-ci.yml fail
exit 0
