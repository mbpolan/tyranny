SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

CREATE SCHEMA IF NOT EXISTS `tyranny_lobby` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci ;
USE `tyranny_lobby` ;

-- -----------------------------------------------------
-- Table `tyranny_lobby`.`users`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tyranny_lobby`.`users` ;

CREATE  TABLE IF NOT EXISTS `tyranny_lobby`.`users` (
  `uid` INT NOT NULL ,
  `username` VARCHAR(45) NULL ,
  `password` VARCHAR(45) NULL ,
  `real_name` VARCHAR(50) NULL ,
  `email` VARCHAR(45) NULL ,
  `bio` VARCHAR(100) NULL ,
  `age` INT NULL ,
  `online` TINYINT(1)  NULL ,
  `muted` TINYINT(1)  NULL ,
  PRIMARY KEY (`uid`) );


-- -----------------------------------------------------
-- Table `tyranny_lobby`.`profiles`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tyranny_lobby`.`profiles` ;

CREATE  TABLE IF NOT EXISTS `tyranny_lobby`.`profiles` (
  `uid` INT NOT NULL ,
  `pid` INT NOT NULL ,
  `real_name` VARCHAR(45) NULL ,
  `age` SMALLINT NULL ,
  `biography` VARCHAR(45) NULL ,
  PRIMARY KEY (`uid`, `pid`) ,
  INDEX `uid` (`uid` ASC) ,
  CONSTRAINT `uid`
    FOREIGN KEY (`uid` )
    REFERENCES `tyranny_lobby`.`users` (`uid` )
    ON DELETE CASCADE
    ON UPDATE CASCADE);


-- -----------------------------------------------------
-- Table `tyranny_lobby`.`userlists`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tyranny_lobby`.`userlists` ;

CREATE  TABLE IF NOT EXISTS `tyranny_lobby`.`userlists` (
  `uid` INT NOT NULL ,
  `other_id` INT NOT NULL ,
  `added` DATETIME NULL ,
  `blocked` TINYINT(1)  NULL ,
  PRIMARY KEY (`uid`, `other_id`) ,
  INDEX `uid-friend_id` (`uid` ASC, `other_id` ASC) ,
  CONSTRAINT `uid-friend_id`
    FOREIGN KEY (`uid` , `other_id` )
    REFERENCES `tyranny_lobby`.`users` (`uid` , `uid` )
    ON DELETE CASCADE
    ON UPDATE CASCADE);


-- -----------------------------------------------------
-- Table `tyranny_lobby`.`bans`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tyranny_lobby`.`bans` ;

CREATE  TABLE IF NOT EXISTS `tyranny_lobby`.`bans` (
  `bid` INT NOT NULL ,
  `uid` INT NULL ,
  `authority` INT NULL ,
  `reason` VARCHAR(45) NULL ,
  `start_date` DATETIME NULL ,
  `expire_date` DATETIME NULL ,
  PRIMARY KEY (`bid`) ,
  INDEX `uid-authority` (`uid` ASC, `authority` ASC) ,
  CONSTRAINT `uid-authority`
    FOREIGN KEY (`uid` , `authority` )
    REFERENCES `tyranny_lobby`.`users` (`uid` , `uid` )
    ON DELETE CASCADE
    ON UPDATE CASCADE);


-- -----------------------------------------------------
-- Table `tyranny_lobby`.`statistics`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tyranny_lobby`.`statistics` ;

CREATE  TABLE IF NOT EXISTS `tyranny_lobby`.`statistics` (
  `uid` INT NOT NULL ,
  `points` INT NULL ,
  `games_played` INT NULL ,
  `won` INT NULL ,
  `lost` INT NULL ,
  PRIMARY KEY (`uid`) ,
  INDEX `uid` (`uid` ASC) ,
  CONSTRAINT `uid`
    FOREIGN KEY (`uid` )
    REFERENCES `tyranny_lobby`.`users` (`uid` )
    ON DELETE CASCADE
    ON UPDATE CASCADE);



SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
