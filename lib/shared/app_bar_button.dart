// Flutter packages
import 'package:flutter/material.dart';

class AppBarButton extends StatelessWidget {
  final IconData icon;
  final Function method;
  const AppBarButton({
    Key? key,
    required this.icon,
    required this.method,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return IconButton(
      icon: Icon(icon),
      alignment: Alignment.center,
      onPressed: () => method(),
    );
  }
}
