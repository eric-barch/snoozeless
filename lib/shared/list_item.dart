// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/theme.dart';

class ListItem extends StatelessWidget {
  final Widget child;
  final Function onTap;
  const ListItem({
    Key? key,
    required this.child,
    required this.onTap,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Container(
      margin: const EdgeInsets.only(top: 15),
      decoration: BoxDecoration(
        border: Border.all(
          width: 3,
          color: appTheme.backgroundColor,
        ),
        borderRadius: const BorderRadius.all(
          Radius.circular(8),
        ),
        color: appTheme.cardColor,
      ),
      child: InkWell(
        onTap: () => onTap(),
        child: Padding(
          padding: const EdgeInsets.only(
            left: 15,
            right: 15,
            top: 20,
            bottom: 20,
          ),
          child: child,
        ),
      ),
    );
  }
}
